#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "GLTFLoader.h"
#include "resources/ResourceManager.h"
#include "resources/meshes/MeshFragment.h"
#include "resources/materials/MaterialDiffuse.h"
#include "graphics/image/ImageLoader.h"

#include "graphics/VulkanHighLevel.h"
#include "GLTFSceneNode.h"


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
using namespace Engine::Core;
using namespace Engine::Core::Loaders;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Resources::Mesh;
using namespace Engine::Resources::Loaders;

GLTFLoader::GLTFLoader(bool loadMaterials, bool useMaterials, const std::string& modelName, const std::string& volumeName) :
bLoadMaterials(loadMaterials), bUseMaterials(useMaterials), srModelName(modelName), srVolumeName(volumeName)
{

}

void GLTFLoader::Load(std::string srPath, std::string srName, std::shared_ptr<Resources::ResourceManager> pResMgr)
{
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    std::string error, warning;
    gltfContext.SetImageLoader(loadImageDataFuncEmpty, nullptr);
    bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, srPath);
    current_primitive = 0;

    m_pMesh = std::make_shared<MeshBase>();
    srModelName = srName;

    if (fileLoaded)
    {
        if (bLoadMaterials)
        {
            LoadTextures(pResMgr, gltfModel);
            LoadMaterials( pResMgr, gltfModel);
        }

        const tinygltf::Scene &scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
        for (size_t i = 0; i < scene.nodes.size(); i++)
        {
            const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
            LoadNode(pResMgr, nullptr, node, scene.nodes[i], gltfModel, 1.0);
        }

        if (gltfModel.animations.size() > 0) 
            LoadAnimations(gltfModel);
        LoadSkins(gltfModel);

        for(auto& node : m_vLinearNodes)
        {
            if(node->m_pMesh)
            {
                node->m_pMesh->SetLocalMatrix(node->GetTransform().GetModel());
                m_pMesh->AddFragment(node->m_pMesh);
            }
        }
    }
}

void GLTFLoader::LoadNode(std::shared_ptr<Resources::ResourceManager> pResMgr, std::shared_ptr<GLTFSceneNode> pParent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, float globalscale)
{
    auto component = std::make_shared<GLTFSceneNode>();
    component->m_index = nodeIndex;
    component->SetName(node.name);
    if(pParent)
        pParent->AddChild(component);

    // Loading position data
    if (node.translation.size() == 3)
        component->GetTransform().pos = glm::make_vec3(node.translation.data());
    // Loading rotation data
    if (node.rotation.size() == 4)
    {
        glm::quat quat = glm::make_quat(node.rotation.data());
        glm::vec3 rot = glm::eulerAngles(quat) * 3.14159f / 180.f;
        component->GetTransform().rot = rot;
    }
    // Loading scale data
    if (node.scale.size() == 3)
        component->GetTransform().scale = glm::make_vec3(node.scale.data());

    // Node with children
    if (node.children.size() > 0)
    {
        for (auto i = 0; i < node.children.size(); i++)
        {
            LoadNode(pResMgr, component, model.nodes[node.children[i]], node.children[i], model, globalscale);
        }
    }

    if (node.mesh > -1) LoadMeshFragment(pResMgr, component, node, model);
    if(node.camera > -1);

    if(!pParent)
        m_vNodes.push_back(component);
    m_vLinearNodes.push_back(component);
}

void GLTFLoader::LoadMeshFragment(std::shared_ptr<Resources::ResourceManager> pResMgr, std::shared_ptr<GLTFSceneNode> sceneNode, const tinygltf::Node &node, const tinygltf::Model &model)
{
    const tinygltf::Mesh mesh = model.meshes[node.mesh];
    auto nativeMesh = std::make_shared<MeshFragment>();
    nativeMesh->SetName(srModelName + "_" + mesh.name);
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
                //vert.normal = glm::vec3(0.f);
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
                //vert.tangent = glm::vec4(0.f);
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
            {
                std::stringstream ss;
                ss << "Index component type " << accessor.componentType << " not supported!";
                utl::logger::log(utl::ELogLevel::eWarning, ss.str());
                return;
            } break;
            }
        }
        
        //RecalculateTangents(vertexBuffer, indexBuffer, vertexStart);

        // Creating primitive for mesh
        Primitive modelPrim{};
        modelPrim.firstIndex = indexStart;
        modelPrim.indexCount = indexCount;
        modelPrim.firstVertex = vertexStart;
        modelPrim.vertexCount = vertexCount;
        modelPrim.bUseMaterial = bUseMaterials;
        modelPrim.setDimensions(posMin, posMax);

        if (bLoadMaterials)
        {
            if(!vMaterials.empty())
                modelPrim.material = primitive.material > -1 ? vMaterials.at(primitive.material) : vMaterials.back();
        }
        else
        {
            if(!vMaterials.empty())
                modelPrim.material = vMaterials.at(current_primitive);
            current_primitive++;
        }

        nativeMesh->AddPrimitive(std::move(modelPrim));

        UVBO->AddMeshData(std::move(vertexBuffer), std::move(indexBuffer));
    }
    sceneNode->m_pMesh = nativeMesh;
    pResMgr->AddExisting(nativeMesh->GetName(), nativeMesh);
}

void GLTFLoader::RecalculateTangents(std::vector<Core::Vertex>& vertices, std::vector<uint32_t>& indices, uint64_t startIndex)
{
    for(auto index = 0; index < indices.size(); index+=3)
    {
        auto& vertex1 = vertices.at(indices.at(index) - startIndex);
        auto& vertex2 = vertices.at(indices.at(index + 1) - startIndex);
        auto& vertex3 = vertices.at(indices.at(index + 2) - startIndex);

        auto vec1 = vertex2.pos - vertex1.pos;
        auto vec2 = vertex3.pos - vertex1.pos;
        glm::vec3 normal = glm::normalize(glm::cross(vec1,vec2));

        glm::vec2 duv1 = vertex2.texcoord - vertex1.texcoord;
        glm::vec2 duv2 = vertex3.texcoord - vertex1.texcoord;

        float k = 1 / (duv1.x * duv2.y - duv2.x * duv1.y);
        glm::mat2x2 UV(duv2.y, -duv1.y, -duv2.x, duv1.x);
        glm::mat2x3 E(vec1, vec2);
        glm::mat2x3 TB = k * E * UV;

        vertex1.tangent += glm::vec4(TB[0], 0.f);
        vertex2.tangent += glm::vec4(TB[0], 0.f);
        vertex3.tangent += glm::vec4(TB[0], 0.f);
        vertex1.normal += normal;
        vertex2.normal += normal;
        vertex3.normal += normal;
    }

    for(auto& vertex : vertices)
    {
        auto& normal = vertex.normal;
        normal = glm::normalize(normal);
        auto& tangent = vertex.tangent;
        tangent = glm::normalize(tangent);
        tangent = glm::vec4(glm::normalize(glm::vec3(tangent) - glm::dot(glm::vec3(tangent), normal) * normal), 0.f);
        //auto w = (glm::dot(glm::cross(normal, tangent), tan2[a]) < 0.0f) ? -1.0f : 1.0f;
    }
}

void GLTFLoader::LoadAnimations(const tinygltf::Model &model)
{
    for (auto& anim : model.animations)
    {
        FAnimation animation{};
		animation.name = anim.name;
		if (anim.name.empty())
			animation.name = std::to_string(m_pMesh->GetAnimations().size());

        for (auto &samp : anim.samplers)
        {
            FAnimationSampler sampler{};

            if (samp.interpolation == "LINEAR")
				sampler.interpolation = FAnimationSampler::EInterpolationType::LINEAR;
			if (samp.interpolation == "STEP")
				sampler.interpolation = FAnimationSampler::EInterpolationType::STEP;
			if (samp.interpolation == "CUBICSPLINE")
				sampler.interpolation = FAnimationSampler::EInterpolationType::CUBICSPLINE;

            // Read sampler input time values
			{
				const tinygltf::Accessor &accessor = model.accessors[samp.input];
				const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
				const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				float *buf = new float[accessor.count];
				memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(float));
				for (size_t index = 0; index < accessor.count; index++)
					sampler.inputs.push_back(buf[index]);

				for (auto input : sampler.inputs) 
                {
					if (input < animation.start)
						animation.start = input;
					if (input > animation.end)
						animation.end = input;
				}
			}

            // Read sampler output T/R/S values 
			{
				const tinygltf::Accessor &accessor = model.accessors[samp.output];
				const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
				const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				switch (accessor.type) 
                {
                    case TINYGLTF_TYPE_VEC3: 
                    {
                        glm::vec3 *buf = new glm::vec3[accessor.count];
                        memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::vec3));
                        for (size_t index = 0; index < accessor.count; index++)
                            sampler.outputsVec4.push_back(glm::vec4(buf[index], 0.0f));
                        break;
                    }
                    case TINYGLTF_TYPE_VEC4: 
                    {
                        glm::vec4 *buf = new glm::vec4[accessor.count];
                        memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::vec4));
                        for (size_t index = 0; index < accessor.count; index++)
                            sampler.outputsVec4.push_back(buf[index]);
                        break;
                    }
                    default: 
                    {
                        utl::logger::log(utl::ELogLevel::eInfo, "unknown type");
                        break;
                    }
				}
			}

            animation.samplers.push_back(sampler);
        }

        // Channels
		for (auto &source: anim.channels) 
        {
			FAnimationChannel channel{};

			if (source.target_path == "rotation")
				channel.path = FAnimationChannel::EPathType::ROTATION;
			if (source.target_path == "translation")
				channel.path = FAnimationChannel::EPathType::TRANSLATION;
			if (source.target_path == "scale")
				channel.path = FAnimationChannel::EPathType::SCALE;

			if (source.target_path == "weights") {
                utl::logger::log(utl::ELogLevel::eInfo, "weights not yet supported, skipping channel");
				continue;
			}
			channel.samplerIndex = source.sampler;
			//channel.node = nodeFromIndex(source.target_node);
			/*if (!channel.node) {
				continue;
			}*/
			animation.channels.push_back(channel);
		}

		m_pMesh->AddAnimation(std::move(animation));
    }
}

void GLTFLoader::LoadMaterials(std::shared_ptr<Resources::ResourceManager> pResMgr, const tinygltf::Model &model)
{
    auto get_texture = [&](const tinygltf::ParameterMap& mat, const std::string& srTexture)
    {
        if (mat.find(srTexture) != mat.end())
        {
            auto index = mat.at(srTexture).TextureIndex();
            return vTextures.at(index);
        }
        
        return pResMgr->Get<Core::Image>("no_texture");
    };

    uint32_t material_index{0};
    for (auto &mat : model.materials)
    {
        std::stringstream ss;
        ss << srModelName << "_"
           << "material"
           << "_";
        if (!mat.name.empty())
            ss << mat.name << "_";
        ss << material_index;

        FMaterialParams params;
        std::shared_ptr<MaterialBase> nativeMaterial = std::make_shared<MaterialDiffuse>();
        nativeMaterial->SetName(ss.str());

        nativeMaterial->AddTexture(ETextureAttachmentType::eBRDFLUT, pResMgr->Get<Core::Image>(srVolumeName + "_brdf"));
        nativeMaterial->AddTexture(ETextureAttachmentType::eIrradiance, pResMgr->Get<Core::Image>(srVolumeName + "_irradiate_cube"));
        nativeMaterial->AddTexture(ETextureAttachmentType::ePrefiltred, pResMgr->Get<Core::Image>(srVolumeName + "_prefiltred_cube"));

        nativeMaterial->AddTexture(ETextureAttachmentType::eDiffuseAlbedo, get_texture(mat.values, "baseColorTexture"));
        nativeMaterial->AddTexture(ETextureAttachmentType::eMetalicRoughness, get_texture(mat.values, "metallicRoughnessTexture"));
        nativeMaterial->AddTexture(ETextureAttachmentType::eSpecularGlossiness, get_texture(mat.values, "specularGlossinessTexture"));
        nativeMaterial->AddTexture(ETextureAttachmentType::eHeight, get_texture(mat.values, "displacementGeometryTexture"));

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

        if(mat.additionalValues.find("displacementGeometry") != mat.additionalValues.end())
            params.alphaCutoff = static_cast<float>(mat.additionalValues.at("displacementGeometry").Factor());

        nativeMaterial->SetParams(std::move(params));
        vMaterials.emplace_back(nativeMaterial);
        pResMgr->AddExisting(nativeMaterial->GetName(), nativeMaterial);
        material_index++;
    }
}

void GLTFLoader::LoadTextures(std::shared_ptr<ResourceManager> pResMgr, const tinygltf::Model &model)
{
    uint32_t index{0};
    for (auto &image : model.images)
    {
        // TODO: Create textures for materials
        std::stringstream ss;
        ss << srModelName << "_"
           << "texture"
           << "_";
        if (!image.name.empty())
            ss << image.name << "_";
        ss << std::to_string(index);

        std::string srPath = "";
        auto texture = LoadTexture(image, srPath);
        //texture->SetName(ss.str());
        vTextures.emplace_back(texture);
        pResMgr->AddExisting(ss.str(), texture);
        index++;
    }
}

std::shared_ptr<Image> GLTFLoader::LoadTexture(const tinygltf::Image &image, std::string path)
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
    auto nativeTexture = std::make_shared<Image>();
    //nativeTexture->SetName(image.name);
    // nativeTexture->LoadFromMemory();

    if (!isKtx)
    {
        auto isAlbedo = image.name.find("albedo") != std::string::npos;
        ImageLoader::AllocateRawDataAsKTXTexture(&texture, &format, image.width, image.height, 1, 2, isAlbedo ? GL_SRGB8_ALPHA8 : GL_RGBA8, true);
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
    for (auto& source : model.skins)
    {
        auto pSkin = std::make_shared<Skin>();
        pSkin->name = source.name;

        // Find skeleton root node
		/*if (source.skeleton > -1)
			pSkin->skeletonRoot = m_vNodes.front()->Find(source.skeleton);

        // Find joint nodes
		for (int jointIndex : source.joints) 
        {
			auto node = m_vNodes.front()->Find(jointIndex);
			if (node)
				pSkin->joints.push_back(m_vNodes.front()->Find(jointIndex));
		}*/

        // Get inverse bind matrices from buffer
		if (source.inverseBindMatrices > -1) 
        {
			const tinygltf::Accessor &accessor = model.accessors[source.inverseBindMatrices];
			const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
			pSkin->inverseBindMatrices.resize(accessor.count);
			memcpy(pSkin->inverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::mat4));
		}
    }
}
