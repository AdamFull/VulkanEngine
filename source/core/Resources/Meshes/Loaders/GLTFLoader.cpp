#include "GLTFLoader.h"
#include "Objects/RenderObject.h"
#include "Objects/Components/GLTFSceneObjectComponent.h"

#include "Resources/Meshes/VulkanMesh.h"
#include "Resources/ResourceManager.h"

#include "Renderer/DataTypes/VulkanVertex.hpp"
#include "Renderer/VulkanVBO.h"
#include "Renderer/VulkanHighLevel.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "external/gltf/tiny_gltf.h"

//Based on https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanglTFModel.cpp
bool loadImageDataFuncEmpty(tinygltf::Image* image, const int imageIndex, std::string* error, std::string* warning, int req_width, int req_height, const unsigned char* bytes, int size, void* userData)
{
    // This function will be used for samples that don't require images to be loaded
    return true;
}

namespace Engine
{
    namespace Loaders
    {
        namespace Model
        {
            void GLTFLoader::Load(std::string srPath, std::shared_ptr<ResourceManager> pResMgr, std::shared_ptr<RenderObject> pRoot)
            {
                tinygltf::Model gltfModel;
                tinygltf::TinyGLTF gltfContext;
                std::string error, warning;
                gltfContext.SetImageLoader(loadImageDataFuncEmpty, nullptr);
                bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, srPath);

                if (fileLoaded) 
                {
                    const tinygltf::Scene &scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
                    for (size_t i = 0; i < scene.nodes.size(); i++) 
                    {
                        const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
                        LoadNode(nullptr, node, scene.nodes[i], gltfModel, 1.0);
                    }
                }
            }

            void GLTFLoader::LoadNode(std::shared_ptr<GLTFSceneObjectComponent> pParent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, float globalscale)
            {
                auto component = std::make_shared<GLTFSceneObjectComponent>();
                component->SetIndex(nodeIndex);
                component->SetParent(pParent);
                component->SetName(node.name);

                //Loading position data
                if (node.translation.size() == 3) 
                    component->SetPosition(glm::make_vec3(node.translation.data()));
                //Loading rotation data
                if (node.rotation.size() == 4) 
                {
                    glm::quat quat = glm::make_quat(node.rotation.data());
                    //component->SetRotation(glm::vec3(quat));
                }
                //Loading scale data
                if (node.scale.size() == 3) 
                    component->SetScale(glm::make_vec3(node.scale.data()));

                // Node with children
                if (node.children.size() > 0) 
                {
                    for (auto i = 0; i < node.children.size(); i++) 
                    {
                        LoadNode(component, model.nodes[node.children[i]], node.children[i], model, globalscale);
                    }
                }

                if (node.mesh > -1)
                {
                    const tinygltf::Mesh mesh = model.meshes[node.mesh];
                    auto nativeMesh = std::make_shared<MeshBase>();
                    nativeMesh->SetName(mesh.name);
                    for (size_t j = 0; j < mesh.primitives.size(); j++)
                    {
                        std::vector<uint32_t> indexBuffer;
                        std::vector<Vertex> vertexBuffer;

                        const tinygltf::Primitive &primitive = mesh.primitives[j];
                        if (primitive.indices < 0) 
                            continue;

                        uint32_t indexStart = UVBO->GetLastIndex();
                        uint32_t vertexStart = UVBO->GetLastVertex();
                        uint32_t indexCount = 0;
                        uint32_t vertexCount = 0;
                        glm::vec3 posMin{};
                        glm::vec3 posMax{};
                        bool hasSkin = false;
                        
                        //Vertices
                        {
                            const float *bufferPos = nullptr;
                            const float *bufferNormals = nullptr;
                            const float *bufferTexCoords = nullptr;
                            const float* bufferColors = nullptr;
                            const float *bufferTangents = nullptr;
                            uint32_t numColorComponents;
                            const uint16_t *bufferJoints = nullptr;
                            const float *bufferWeights = nullptr;

                            // Position attribute is required
				            assert(primitive.attributes.find("POSITION") != primitive.attributes.end());
                            const tinygltf::Accessor &posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
                            const tinygltf::BufferView &posView = model.bufferViews[posAccessor.bufferView];
                            bufferPos = reinterpret_cast<const float *>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
                            posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
                            posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);

                            // Load model normals
                            if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
                            {
                                const tinygltf::Accessor &normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                                const tinygltf::BufferView &normView = model.bufferViews[normAccessor.bufferView];
                                bufferNormals = reinterpret_cast<const float *>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
                            }

                            // Load UV coordinates
                            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
                            {
                                const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                                const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
                                bufferTexCoords = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                            }

                            // Load mesh color data
                            if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
                            {
                                const tinygltf::Accessor &colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
                                const tinygltf::BufferView &colorView = model.bufferViews[colorAccessor.bufferView];
                                // Color buffer are either of type vec3 or vec4
                                numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
                                bufferColors = reinterpret_cast<const float *>(&(model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
                            }

                            // Load tangent
                            if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
                            {
                                const tinygltf::Accessor &tangentAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
                                const tinygltf::BufferView &tangentView = model.bufferViews[tangentAccessor.bufferView];
                                bufferTangents = reinterpret_cast<const float *>(&(model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
                            }

                            // Load joints
                            if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end())
                            {
                                const tinygltf::Accessor &jointAccessor = model.accessors[primitive.attributes.find("JOINTS_0")->second];
                                const tinygltf::BufferView &jointView = model.bufferViews[jointAccessor.bufferView];
                                bufferJoints = reinterpret_cast<const uint16_t *>(&(model.buffers[jointView.buffer].data[jointAccessor.byteOffset + jointView.byteOffset]));
                            }

                            // Load weight
                            if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end())
                            {
                                const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("WEIGHTS_0")->second];
                                const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
                                bufferWeights = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                            }

                            hasSkin = (bufferJoints && bufferWeights);

				            vertexCount = static_cast<uint32_t>(posAccessor.count);

                            for (size_t v = 0; v < posAccessor.count; v++)
                            {
                                Vertex vert{};
                                vert.pos = glm::vec4(glm::make_vec3(&bufferPos[v * 3]), 1.0f);
                                vert.normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
                                vert.texcoord = bufferTexCoords ? glm::make_vec2(&bufferTexCoords[v * 2]) : glm::vec3(0.0f);
                                if (bufferColors)
                                {
                                    switch (numColorComponents)
                                    {
                                    case 3:
                                        vert.color = glm::vec4(glm::make_vec3(&bufferColors[v * 3]), 1.0f);
                                    case 4:
                                        vert.color = glm::make_vec4(&bufferColors[v * 4]);
                                    }
                                }
                                else
                                {
                                    vert.color = glm::vec4(1.0f);
                                }
                                vert.tangent = bufferTangents ? glm::vec4(glm::make_vec4(&bufferTangents[v * 4])) : glm::vec4(0.0f);
                                //TODO: Add skinning
                                //vert.joint0 = hasSkin ? glm::vec4(glm::make_vec4(&bufferJoints[v * 4])) : glm::vec4(0.0f);
                                //vert.weight0 = hasSkin ? glm::make_vec4(&bufferWeights[v * 4]) : glm::vec4(0.0f);
                                vertexBuffer.push_back(vert);
                            }
                        }

                        // Indices
                        {
                            const tinygltf::Accessor &accessor = model.accessors[primitive.indices];
                            const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
                            const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];

                            indexCount = static_cast<uint32_t>(accessor.count);

                            switch (accessor.componentType)
                            {
                            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                            {
                                uint32_t *buf = new uint32_t[accessor.count];
                                memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
                                for (size_t index = 0; index < accessor.count; index++)
                                    indexBuffer.push_back(buf[index] + vertexStart);
                                delete[] buf;
                                break;
                            }
                            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                            {
                                uint16_t *buf = new uint16_t[accessor.count];
                                memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
                                for (size_t index = 0; index < accessor.count; index++)
                                    indexBuffer.push_back(buf[index] + vertexStart);
                                delete[] buf;
                                break;
                            }
                            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                            {
                                uint8_t *buf = new uint8_t[accessor.count];
                                memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
                                for (size_t index = 0; index < accessor.count; index++)
                                    indexBuffer.push_back(buf[index] + vertexStart);
                                delete[] buf;
                                break;
                            }
                            default:
                                std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
                                return;
                            }
                        }

                        // Creating primitive for mesh
                        Primitive modelPrim{};
                        modelPrim.firstIndex = indexStart;
                        modelPrim.indexCount = indexCount;
                        modelPrim.firstVertex = vertexStart;
                        modelPrim.vertexCount = vertexCount;
                        modelPrim.setDimensions(posMin, posMax);
                        nativeMesh->AddPrimitive(std::move(modelPrim));

                        UVBO->AddMeshData(std::move(vertexBuffer), std::move(indexBuffer));
                    }
                    component->SetMesh(nativeMesh);
                }
            }

            void GLTFLoader::LoadMaterials(std::shared_ptr<ResourceManager> pResMgr, const tinygltf::Model &model)
            {
                for (auto& mat : model.materials) 
                {
                    //TODO: Add material factory
                    auto new_material = std::make_shared<MaterialBase>();
                    if (mat.values.find("baseColorTexture") != mat.values.end())
                    {
                        mat.values.at("baseColorTexture").TextureIndex();
                        model.textures;
                        //TODO: Create unique texture name
                        //pResMgr->Get<TextureBase>();
                    }
                    if (mat.values.find("metallicRoughnessTexture") != mat.values.end())
                    {
                        //material.metallicRoughnessTexture = getTexture(gltfModel.textures[mat.values["metallicRoughnessTexture"].TextureIndex()].source);
                    }
                    if (mat.values.find("roughnessFactor") != mat.values.end())
                    {
                        //material.roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
                    }
                    if (mat.values.find("metallicFactor") != mat.values.end())
                    {
                        //material.metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
                    }
                    if (mat.values.find("baseColorFactor") != mat.values.end())
                    {
                        //material.baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
                    }
                    if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end())
                    {
                        //material.normalTexture = getTexture(gltfModel.textures[mat.additionalValues["normalTexture"].TextureIndex()].source);
                    }
                    else
                    {
                        //material.normalTexture = &emptyTexture;
                    }
                    if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end())
                    {
                       // material.emissiveTexture = getTexture(gltfModel.textures[mat.additionalValues["emissiveTexture"].TextureIndex()].source);
                    }
                    if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end())
                    {
                        //material.occlusionTexture = getTexture(gltfModel.textures[mat.additionalValues["occlusionTexture"].TextureIndex()].source);
                    }
                    if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end())
                    {
                        /*tinygltf::Parameter param = mat.additionalValues["alphaMode"];
                        if (param.string_value == "BLEND")
                        {
                            material.alphaMode = Material::ALPHAMODE_BLEND;
                        }
                        if (param.string_value == "MASK")
                        {
                            material.alphaMode = Material::ALPHAMODE_MASK;
                        }*/
                    }
                    if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end())
                    {
                        //material.alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
                    }

                    pResMgr->AddExisting(mat.name, new_material);
                }
            }

            void GLTFLoader::LoadTextures(std::shared_ptr<ResourceManager> pResMgr, const tinygltf::Model &model)
            {
                for (auto& image : model.images)
                {
                    //TODO: Create textures for materials
                }
            }

        }
    }
}