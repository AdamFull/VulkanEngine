#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "GLTFLoader.h"
#include "Objects/RenderObject.h"
#include "Objects/Components/MeshComponentBase.h"

#include "Resources/Meshes/VulkanMesh.h"
#include "Resources/ResourceManager.h"
#include "Resources/Textures/ImageLoader.h"
#include "Resources/Materials/MaterialDiffuse.h"

#include "Core/DataTypes/VulkanVertex.hpp"
#include "Core/VulkanVBO.h"
#include "Core/VulkanHighLevel.h"

// Based on https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanglTFModel.cpp
bool loadImageDataFuncEmpty(tinygltf::Image *image, const int imageIndex, std::string *error, std::string *warning, int req_width, int req_height, const unsigned char *bytes, int size, void *userData)
{
    // KTX files will be handled by our own code
    if (image->uri.find_last_of(".") != std::string::npos)
    {
        if (image->uri.substr(image->uri.find_last_of(".") + 1) == "ktx")
        {
            return true;
        }
    }

    return tinygltf::LoadImageData(image, imageIndex, error, warning, req_width, req_height, bytes, size, userData);
}

using namespace Engine;
using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;
using namespace Engine::Resources::Mesh;
using namespace Engine::Resources::Loaders;

uint32_t GLTFLoader::current_primitive{0};

std::shared_ptr<Objects::RenderObject> GLTFLoader::Load(std::string srPath, std::string srName, std::shared_ptr<LoaderTemporaryObject> pTmp, std::shared_ptr<Resources::ResourceManager> pResMgr)
{
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    std::string error, warning;
    gltfContext.SetImageLoader(loadImageDataFuncEmpty, nullptr);
    bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, srPath);
    current_primitive = 0;

    pTmp->srModelName = srName;
    auto pRootComponent = std::make_shared<Objects::RenderObject>();
    pRootComponent->SetName(srName);

    if (fileLoaded)
    {
        if (pTmp->bLoadMaterials)
        {
            LoadTextures(pTmp, pResMgr, gltfModel);
            LoadMaterials(pTmp, pResMgr, gltfModel);
        }

        const tinygltf::Scene &scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
        for (size_t i = 0; i < scene.nodes.size(); i++)
        {
            const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
            LoadNode(pTmp, pRootComponent, node, scene.nodes[i], gltfModel, 1.0);
        }
    }

    return pRootComponent;
}

void GLTFLoader::LoadNode(std::shared_ptr<LoaderTemporaryObject> tmp, std::shared_ptr<Objects::RenderObject> pParent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, float globalscale)
{
    auto component = std::make_shared<Objects::Components::MeshComponentBase>();
    component->SetIndex(nodeIndex);
    component->SetName(node.name);
    pParent->AddChild(component);

    // Loading position data
    if (node.translation.size() == 3)
        component->SetPosition(glm::make_vec3(node.translation.data()));
    // Loading rotation data
    if (node.rotation.size() == 4)
    {
        glm::quat quat = glm::make_vec4(node.rotation.data());
        glm::vec3 rot = glm::eulerAngles(quat) * 3.14159f / 180.f;
        component->SetRotation(rot);
    }
    // Loading scale data
    if (node.scale.size() == 3)
        component->SetScale(glm::make_vec3(node.scale.data()));

    // Node with children
    if (node.children.size() > 0)
    {
        for (auto i = 0; i < node.children.size(); i++)
        {
            LoadNode(tmp, component, model.nodes[node.children[i]], node.children[i], model, globalscale);
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
            std::vector<Core::Vertex> vertexBuffer;

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

            // Vertices
            {
                const float *bufferPos = nullptr;
                const float *bufferNormals = nullptr;
                const float *bufferTexCoords = nullptr;
                const float *bufferColors = nullptr;
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
                    Core::Vertex vert{};
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
                    // TODO: Add skinning
                    vert.joint0 = hasSkin ? glm::vec4(glm::make_vec4(&bufferJoints[v * 4])) : glm::vec4(0.0f);
                    vert.weight0 = hasSkin ? glm::make_vec4(&bufferWeights[v * 4]) : glm::vec4(0.0f);
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

            if (tmp->bLoadMaterials)
                modelPrim.material = primitive.material > -1 ? tmp->vMaterials.at(primitive.material) : tmp->vMaterials.back();
            else
            {
                modelPrim.material = tmp->vMaterials.at(current_primitive);
                current_primitive++;
            }

            nativeMesh->AddPrimitive(std::move(modelPrim));

            UVBO->AddMeshData(std::move(vertexBuffer), std::move(indexBuffer));
        }
        component->SetMesh(nativeMesh);
    }
}

void GLTFLoader::LoadMaterials(std::shared_ptr<LoaderTemporaryObject> tmp, std::shared_ptr<Resources::ResourceManager> pResMgr, const tinygltf::Model &model)
{
    auto get_texture = [&tmp, &pResMgr](const tinygltf::ParameterMap& mat, const std::string& srTexture)
    {
        if (mat.find(srTexture) != mat.end())
        {
            auto index = mat.at(srTexture).TextureIndex();
            return tmp->vTextures.at(index);
        }
        
        return pResMgr->Get<Texture::TextureBase>("no_texture");
    };

    uint32_t material_index{0};
    for (auto &mat : model.materials)
    {
        std::stringstream ss;
        ss << tmp->srModelName << "_"
           << "material"
           << "_";
        if (!mat.name.empty())
            ss << mat.name << "_";
        ss << material_index;

        FMaterialParams params;
        std::shared_ptr<MaterialBase> nativeMaterial = std::make_shared<MaterialDiffuse>();
        nativeMaterial->SetName(ss.str());

        //nativeMaterial->AddTexture(ETextureAttachmentType::eDiffuseAlbedo, get_texture(mat.values, "baseColorTexture"));
        nativeMaterial->AddTexture(ETextureAttachmentType::eDiffuseAlbedo, pResMgr->Get<Texture::TextureBase>("brdf_test"));
        nativeMaterial->AddTexture(ETextureAttachmentType::eMetalicRoughness, get_texture(mat.values, "metallicRoughnessTexture"));
        nativeMaterial->AddTexture(ETextureAttachmentType::eSpecularGlossiness, get_texture(mat.values, "specularGlossinessTexture"));

        if (mat.values.find("roughnessFactor") != mat.values.end())
            params.roughnessFactor = static_cast<float>(mat.values.at("roughnessFactor").Factor());

        if (mat.values.find("metallicFactor") != mat.values.end())
            params.metallicFactor = static_cast<float>(mat.values.at("metallicFactor").Factor());

        if (mat.values.find("baseColorFactor") != mat.values.end())
            params.baseColorFactor = glm::make_vec4(mat.values.at("baseColorFactor").ColorFactor().data());

        nativeMaterial->AddTexture(ETextureAttachmentType::eNormal, get_texture(mat.additionalValues, "normalTexture"));
        nativeMaterial->AddTexture(ETextureAttachmentType::eEmissive, get_texture(mat.additionalValues, "emissiveTexture"));
        nativeMaterial->AddTexture(ETextureAttachmentType::eOcclusion, get_texture(mat.additionalValues, "occlusionTexture"));

        if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end())
        {
            tinygltf::Parameter param = mat.additionalValues.at("alphaMode");
            if (param.string_value == "BLEND")
                params.alphaMode = FMaterialParams::EAlphaMode::EBLEND;
            if (param.string_value == "MASK")
                params.alphaMode = FMaterialParams::EAlphaMode::EMASK;
        }
        if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end())
            params.alphaCutoff = static_cast<float>(mat.additionalValues.at("alphaCutoff").Factor());

        nativeMaterial->SetParams(std::move(params));
        nativeMaterial->AddTexture(ETextureAttachmentType::eEmpty, pResMgr->Get<TextureBase>("no_texture"));
        tmp->vMaterials.emplace_back(nativeMaterial);
        pResMgr->AddExisting(nativeMaterial->GetName(), nativeMaterial);
        material_index++;
    }
}

void GLTFLoader::LoadTextures(std::shared_ptr<LoaderTemporaryObject> tmp, std::shared_ptr<ResourceManager> pResMgr, const tinygltf::Model &model)
{
    uint32_t index{0};
    for (auto &image : model.images)
    {
        // TODO: Create textures for materials
        std::stringstream ss;
        ss << tmp->srModelName << "_"
           << "texture"
           << "_";
        if (!image.name.empty())
            ss << image.name << "_";
        ss << std::to_string(index);

        std::string srPath = "";
        auto texture = LoadTexture(image, srPath);
        texture->SetName(ss.str());
        tmp->vTextures.emplace_back(texture);
        pResMgr->AddExisting(texture->GetName(), texture);
        index++;
    }
}

std::shared_ptr<TextureBase> GLTFLoader::LoadTexture(const tinygltf::Image &image, std::string path)
{
    bool isKtx = false;
    // Image points to an external ktx file
    if (image.uri.find_last_of(".") != std::string::npos)
    {
        if (image.uri.substr(image.uri.find_last_of(".") + 1) == "ktx")
            isKtx = true;
    }

    vk::Format format;
    ktxTexture *texture;
    auto nativeTexture = std::make_shared<TextureBase>();
    nativeTexture->SetName(image.name);
    // nativeTexture->LoadFromMemory();

    if (!isKtx)
    {
        ImageLoader::AllocateRawDataAsKTXTexture(&texture, &format, image.width, image.height, 1, 2, GL_SRGB8_ALPHA8, true);
        vk::DeviceSize bufferSize = 0;
        bool deleteBuffer = false;
        if (image.component == 3)
        {
            // Most devices don't support RGB only on Vulkan so convert if necessary
            // TODO: Check actual format support and transform only if required
            bufferSize = image.width * image.height * 4;
            texture->pData = static_cast<unsigned char *>(calloc(bufferSize, sizeof(unsigned char)));
            unsigned char *rgba = texture->pData;
            unsigned char *rgb{nullptr};
            memcpy(rgb, image.image.data(), bufferSize);

#pragma omp parallel for
            for (size_t i = 0; i < image.width * image.height; ++i)
            {
                for (int32_t j = 0; j < 3; ++j)
                {
                    rgba[j] = rgb[j];
                }
                rgba += 4;
                rgb += 3;
            }
            deleteBuffer = true;
        }
        else
        {
            bufferSize = image.image.size();
            texture->pData = static_cast<unsigned char *>(calloc(bufferSize, sizeof(unsigned char)));
            memcpy(texture->pData, image.image.data(), bufferSize);
        }
    }
    else
    {
        std::string srTexturePath = path + "/" + image.uri;
        ImageLoader::Load(srTexturePath.c_str(), &texture, &format);
    }
    nativeTexture->LoadFromMemory(texture, format);
    ImageLoader::Close(&texture);

    return nativeTexture;
}

void GLTFLoader::LoadSkins(const tinygltf::Model &model)
{
    /*for (tinygltf::Skin &source : gltfModel.skins)
    {

    }*/
}
