#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE

#include "GLTFLoader.h"
#include "resources/ResourceManager.h"
#include "resources/materials/MaterialLoader.h"
#include "filesystem/FilesystemHelper.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/scene/objects/components/CameraManager.h"
#include "graphics/scene/objects/components/LightSourceManager.h"


// Based on https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanglTFModel.cpp
bool loadImageDataFuncEmpty(tinygltf::Image *image, const int imageIndex, std::string *error, std::string *warning, int req_width, int req_height, const unsigned char *bytes, int size, void *userData)
{
    // KTX files will be handled by our own code
    if (image->uri.find_last_of(".") != std::string::npos)
    {
        if (image->uri.substr(image->uri.find_last_of(".") + 1) == "ktx")
            return true;
        if (image->uri.substr(image->uri.find_last_of(".") + 1) == "ktx2")
            return true;

    }

    return false;//tinygltf::LoadImageData(image, imageIndex, error, warning, req_width, req_height, bytes, size, userData);
}

using namespace engine;
using namespace engine::core;
using namespace engine::core::scene;
using namespace engine::resources;
using namespace engine::resources::material;
using namespace engine::resources::loaders;

GLTFLoader::GLTFLoader(bool loadMaterials, bool useMaterials, const std::string& modelName, const std::string& volumeName) :
bLoadMaterials(loadMaterials), bUseMaterials(useMaterials), srModelName(modelName), srVolumeName(volumeName)
{

}

//TODO: load animations
//TODO: load cameras
//TODO: load lights
//TODO: load skins
void GLTFLoader::load(ref_ptr<core::scene::CRenderObject>& pParent, const std::string& srPath, const std::string& srName)
{
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    std::string error, warning;
    gltfContext.SetImageLoader(loadImageDataFuncEmpty, nullptr);
    auto fpath = FilesystemHelper::getWorkDir() / srPath;
    fsParentPath = fs::weakly_canonical(fpath.parent_path());
    fsParentPath = fs::relative(fsParentPath, FilesystemHelper::getWorkDir());
    bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, fpath.string());
    current_primitive = 0;

    if(!warning.empty())
        utl::logger::log(utl::ELogLevel::eWarning, warning);
    if(!error.empty())
        utl::logger::log(utl::ELogLevel::eError, error);

    srModelName = srName;

    if (fileLoaded)
    {
        if (bLoadMaterials)
        {
            loadTextures(gltfModel);
            loadMaterials(gltfModel);
        }

        const tinygltf::Scene &scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
        //pParent->setName(scene.name);
        for (size_t i = 0; i < scene.nodes.size(); i++)
        {
            const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
            loadNode(pParent, node, scene.nodes[i], gltfModel, 1.0);
        }

        if (gltfModel.animations.size() > 0) 
            loadAnimations(gltfModel);
        loadSkins(gltfModel);
    }
}

void GLTFLoader::loadNode(ref_ptr<CRenderObject>& pParent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, float globalscale)
{
    auto component = make_ref<CRenderObject>();
    component->setIndex(nodeIndex);

    if(!node.name.empty())
        component->setName(node.name);
    else
    {
        std::stringstream ss;
        ss << srModelName << "_" << nodeIndex;
        component->setName(ss.str());
    }

    if(pParent)
        pParent->addChild(component);

    //TODO: incorrect rotation
    auto& transform = component->getLocalTransform();
    // Loading position data
    if (node.translation.size() == 3)
        transform.setPosition(glm::make_vec3(node.translation.data()));
    // Loading rotation data
    if (node.rotation.size() == 4)
    {
        glm::quat quat = glm::conjugate(glm::make_quat(node.rotation.data()));
        glm::vec3 rot = glm::eulerAngles(quat) * glm::vec3(-1.0, 1.0, 1.0);
        transform.setRotation(rot);
    }
    // Loading scale data
    if (node.scale.size() == 3)
        transform.setScale(glm::make_vec3(node.scale.data()));

    // Node with children
    if (node.children.size() > 0)
    {
        for (auto i = 0; i < node.children.size(); i++)
            loadNode(component, model.nodes[node.children[i]], node.children[i], model, globalscale);
    }

    if (node.mesh > -1) loadMeshFragment(component, node, model);
    if(node.camera > -1) loadCamera(component, node, model); 
    if(node.skin > -1) loadSkin(component, node, model);

    if(!node.extensions.empty())
    {
        auto light_support = node.extensions.find("KHR_lights_punctual");
        if(light_support != node.extensions.end())
        {
            auto& extension = light_support->second;
            auto source = extension.Get("light");
            loadLight(component, source.GetNumberAsInt(), node, model);
        }
    }
}

void GLTFLoader::loadMeshFragment(ref_ptr<CRenderObject>& sceneNode, const tinygltf::Node &node, const tinygltf::Model &model)
{
    const tinygltf::Mesh mesh = model.meshes[node.mesh];
    auto nativeMesh = make_ref<CMeshComponent>();
    if(!mesh.name.empty())
        nativeMesh->setName(mesh.name);
    else
    {
        std::stringstream ss{};
        ss << srModelName << "_mesh_" << node.mesh;
        nativeMesh->setName(ss.str());
    }
    
    for (size_t j = 0; j < mesh.primitives.size(); j++)
    {
        std::vector<uint32_t> indexBuffer;
        std::vector<FVertex> vertexBuffer;

        const tinygltf::Primitive &primitive = mesh.primitives[j];
        if (primitive.indices < 0)
            continue;

        uint32_t indexStart = CVBO::inst()->getLastIndex();
        uint32_t vertexStart = CVBO::inst()->getLastVertex();
        uint32_t indexCount = 0;
        uint32_t vertexCount = 0;
        glm::vec3 posMin{};
        glm::vec3 posMax{};
        bool bHasSkin{false}, bHasNormals{false}, bHasTangents{false};

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
                bHasNormals = true;
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
                bHasTangents = true;
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

            bHasSkin = (bufferJoints && bufferWeights);

            vertexCount = static_cast<uint32_t>(posAccessor.count);

            for (size_t v = 0; v < posAccessor.count; v++)
            {
                FVertex vert{};
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
                //vert.joint0 = bHasSkin ? glm::vec4(glm::make_vec4(&bufferJoints[v * 4])) : glm::vec4(0.0f);
                //vert.weight0 = bHasSkin ? glm::make_vec4(&bufferWeights[v * 4]) : glm::vec4(0.0f);
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
        FPrimitive modelPrim{};
        modelPrim.firstIndex = indexStart;
        modelPrim.indexCount = indexCount;
        modelPrim.firstVertex = vertexStart;
        modelPrim.vertexCount = vertexCount;
        modelPrim.bUseMaterial = bUseMaterials;
        modelPrim.setDimensions(posMin, posMax);

        if (bLoadMaterials)
        {
            if(!vMaterials.empty())
            {
                auto& material = primitive.material > -1 ? vMaterials.at(primitive.material) : vMaterials.back();
                material->incrementUsageCount();
                if(bHasNormals) material->addDefinition("HAS_NORMALS", ""); 
                if(bHasTangents) material->addDefinition("HAS_TANGENTS", ""); 
                modelPrim.material = material;
            }
        }
        else
        {
            if(!vMaterials.empty())
                modelPrim.material = vMaterials.at(current_primitive);
            current_primitive++;
        }

        nativeMesh->addPrimitive(std::move(modelPrim));
        CVBO::inst()->addMeshData(std::move(vertexBuffer), std::move(indexBuffer));
    }
    sceneNode->setMesh(std::move(nativeMesh));
    //CResourceManager::inst()->addExisting(nativeMesh->getName(), nativeMesh);

    std::map<int32_t, int32_t> materialInstances{};
}

void GLTFLoader::loadCamera(ref_ptr<core::scene::CRenderObject>& sceneNode, const tinygltf::Node &node, const tinygltf::Model &model)
{
    const tinygltf::Camera camera = model.cameras[node.camera];
    auto nativeCamera = make_ref<CCameraComponent>();
    if(!camera.name.empty())
        nativeCamera->setName(camera.name);
    else
    {
        std::stringstream ss{};
        ss << srModelName << "_camera_" << node.camera;
        nativeCamera->setName(ss.str());
    }

    if(camera.type == "orthographic")
    {
        camera.orthographic.xmag;
        camera.orthographic.ymag;
        nativeCamera->setNearPlane(camera.orthographic.znear);
        nativeCamera->setFarPlane(camera.orthographic.zfar);
    }
    else if(camera.type == "perspective")
    {
        nativeCamera->setFieldOfView(camera.perspective.yfov);
        nativeCamera->setNearPlane(camera.perspective.znear);
        nativeCamera->setFarPlane(camera.perspective.zfar);
    }

    sceneNode->setCamera(std::move(nativeCamera));
    CCameraManager::inst()->attach(sceneNode);
}

void GLTFLoader::loadSkin(ref_ptr<core::scene::CRenderObject>& sceneNode, const tinygltf::Node &node, const tinygltf::Model &model)
{
    const tinygltf::Skin skin = model.skins[node.skin];
    /*auto nativeSkin = make_ref<CSkinComponent>();
    if(!skin.name.empty())
        nativeSkin->setName(skin.name);
    else
    {
        std::stringstream ss{};
        ss << srModelName << "_skin_" << node.skin;
        nativeSkin->setName(ss.str());
    }*/
}

void GLTFLoader::loadLight(ref_ptr<core::scene::CRenderObject>& sceneNode, uint32_t light_index, const tinygltf::Node &node, const tinygltf::Model &model)
{
    const tinygltf::Light light = model.lights[light_index];
    auto nativeLight = make_ref<CLightComponent>();
    if(!light.name.empty())
        nativeLight->setName(light.name);
    else
    {
        std::stringstream ss{};
        ss << srModelName << "_light_" << light_index;
        nativeLight->setName(ss.str());
    }

    if(light.color.empty())
        nativeLight->setColor(glm::vec3(1.f));
    else
        nativeLight->setColor(glm::make_vec3(light.color.data()));

    nativeLight->setIntencity(light.intensity);

    if(light.type == "spot")
    {
        nativeLight->setType(ELightSourceType::eSpot);
        nativeLight->setInnerAngle(light.spot.innerConeAngle);
        nativeLight->setOuterAngle(light.spot.outerConeAngle);
    }
    else if(light.type == "point")
        nativeLight->setType(ELightSourceType::ePoint);
    else if(light.type == "directional")
        nativeLight->setType(ELightSourceType::eDirectional);

    sceneNode->setLight(std::move(nativeLight));
    CLightSourceManager::inst()->addLight(sceneNode);
}

void GLTFLoader::recalculateTangents(std::vector<FVertex>& vertices, std::vector<uint32_t>& indices, uint64_t startIndex)
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

void GLTFLoader::loadAnimations(const tinygltf::Model &model)
{
    for (auto& anim : model.animations)
    {
        FAnimation animation{};
		animation.name = anim.name;
		//if (anim.name.empty())
		//	animation.name = std::to_string(m_pMesh->getAnimations().size());

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

        //TODO: save animations to mesh
		//m_pMesh->addAnimation(std::move(animation));
    }
}

void GLTFLoader::loadMaterials(const tinygltf::Model &model)
{
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
        ref_ptr<CMaterialBase> nativeMaterial = CMaterialLoader::inst()->create("default");
        nativeMaterial->setName(ss.str());

        if (mat.values.find("baseColorTexture") != mat.values.end())
        {
            auto texture = mat.values.at("baseColorTexture");
            nativeMaterial->addTexture("color_tex", vTextures.at(texture.TextureIndex()));
            nativeMaterial->addDefinition("HAS_BASECOLORMAP", "");
        }

        if (mat.values.find("metallicRoughnessTexture") != mat.values.end())
        {
            auto texture = mat.values.at("metallicRoughnessTexture");
            nativeMaterial->addTexture("rmah_tex", vTextures.at(texture.TextureIndex()));
            nativeMaterial->addDefinition("HAS_METALLIC_ROUGHNESS", "");
        }

        if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end())
        {
            auto texture = mat.additionalValues.at("normalTexture");
            params.normalScale = static_cast<float>(texture.TextureScale());
            nativeMaterial->addTexture("normal_tex", vTextures.at(texture.TextureIndex()));
            nativeMaterial->addDefinition("HAS_NORMALMAP", "");
        }

        if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end())
        {
            auto texture = mat.additionalValues.at("occlusionTexture");
            params.occlusionStrenth = static_cast<float>(texture.TextureStrength());
            nativeMaterial->addTexture("occlusion_tex", vTextures.at(texture.TextureIndex()));
            nativeMaterial->addDefinition("HAS_OCCLUSIONMAP", "");
        }

        if (mat.additionalValues.find("displacementGeometryTexture") != mat.additionalValues.end())
        {
            auto texture = mat.additionalValues.at("displacementGeometryTexture");

            const auto level = texture.json_double_value.find("level");
            if(level != std::end(texture.json_double_value))
                params.tessLevel = level->second;
            const auto strength = texture.json_double_value.find("strength");
            if(strength != std::end(texture.json_double_value))
                params.tessStrength = strength->second;

            nativeMaterial->addTexture("height_tex", vTextures.at(texture.TextureIndex()));
            nativeMaterial->addDefinition("HAS_HEIGHTMAP", "");
        }

        if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end())
        {
            auto texture = mat.additionalValues.at("emissiveTexture");
            nativeMaterial->addTexture("emissive_tex", vTextures.at(texture.TextureIndex()));
            nativeMaterial->addDefinition("HAS_EMISSIVEMAP", "");
        }

        if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end())
            params.emissiveFactor = glm::make_vec3(mat.additionalValues.at("emissiveFactor").ColorFactor().data()); 

        // TODO: add HAS_HEIGHTMAP

        if (mat.values.find("roughnessFactor") != mat.values.end())
            params.roughnessFactor = static_cast<float>(mat.values.at("roughnessFactor").Factor());

        if (mat.values.find("metallicFactor") != mat.values.end())
            params.metallicFactor = static_cast<float>(mat.values.at("metallicFactor").Factor());

        if (mat.values.find("baseColorFactor") != mat.values.end())
            params.baseColorFactor = glm::make_vec4(mat.values.at("baseColorFactor").ColorFactor().data());

        if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end())
        {
            tinygltf::Parameter param = mat.additionalValues.at("alphaMode");
            if (param.string_value == "BLEND")
                params.alphaMode = FMaterialParams::EAlphaMode::EBLEND;
            if (param.string_value == "MASK")
                params.alphaMode = FMaterialParams::EAlphaMode::EMASK;
        }

        if (mat.additionalValues.find("doubleSided") != mat.additionalValues.end())
        {
            //TODO: save it somewhere
            if(mat.additionalValues.at("doubleSided").bool_value)
            {
                auto& pipeline = nativeMaterial->getPipeline();
                pipeline->setCulling(vk::CullModeFlagBits::eNone);
            }
        }
            

        if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end())
            params.alphaCutoff = static_cast<float>(mat.additionalValues.at("alphaCutoff").Factor());             

        //if(mat.additionalValues.find("displacementGeometry") != mat.additionalValues.end())
        //    params.alphaCutoff = static_cast<float>(mat.additionalValues.at("displacementGeometry").Factor());

        nativeMaterial->setParams(std::move(params));
        vMaterials.emplace_back(nativeMaterial);
        CResourceManager::inst()->addExisting(nativeMaterial->getName(), nativeMaterial);
        material_index++;
    }
}

void GLTFLoader::loadTextures(const tinygltf::Model &model)
{
    for(auto& texture : model.textures)
    {
        auto image_index = texture.source;
        if(image_index < 0 && !texture.extensions.empty())
        {
            auto basisu_support = texture.extensions.find("KHR_texture_basisu");
            if(basisu_support != texture.extensions.end())
            {
                auto& extension = basisu_support->second;
                auto source = extension.Get("source");
                image_index = source.GetNumberAsInt();
            }
        }

        auto& image = model.images.at(image_index);
        std::string tex_name = image.name;
        if (tex_name.empty())
        {
            std::stringstream ss;
            ss << srModelName << "_" << "texture_" << image_index;
            tex_name = ss.str();
        }

        auto texture_object = loadTexture(image, fsParentPath);
        vTextures.emplace_back(texture_object);
        CResourceManager::inst()->addExisting(tex_name, texture_object);
    }
}

ref_ptr<CImage> GLTFLoader::loadTexture(const tinygltf::Image &image, const fs::path& path)
{
    auto nativeTexture = make_ref<CImage>();
    auto realPath = fs::weakly_canonical(path / image.uri);
    nativeTexture->create(realPath);
    return nativeTexture;
}

void GLTFLoader::loadSkins(const tinygltf::Model &model)
{
    for (auto& source : model.skins)
    {
        auto pSkin = make_ref<FSkin>();
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
