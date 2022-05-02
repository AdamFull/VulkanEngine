#pragma once
#include "external/tinygltf/tiny_gltf.h"
#include "resources/meshes/VulkanMesh.h"
#include "graphics/data_types/VulkanVertex.hpp"

namespace Engine
{
    namespace Core
    {
        class CImage;
    }
    namespace Resources
    {
        class ResourceManager;
        namespace Material { class MaterialBase; }

        namespace Loaders
        {
            class GLTFSceneNode;
            
            struct GLTFLoader
            {
            public:
                GLTFLoader(bool loadMaterials, bool useMaterials, const std::string& modelName, const std::string& volumeName);

                void Load(std::string srPath, std::string srName, std::shared_ptr<Resources::ResourceManager> pResMgr);

                inline void AddMaterial(std::shared_ptr<Material::MaterialBase> material) { vMaterials.emplace_back(material); }

                inline const std::shared_ptr<Mesh::MeshBase> GetMesh() const { return m_pMesh; }

            private:
                void LoadNode(std::shared_ptr<Resources::ResourceManager> pResMgr, std::shared_ptr<GLTFSceneNode> pParent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, float globalscale);

                void LoadMeshFragment(std::shared_ptr<Resources::ResourceManager> pResMgr, std::shared_ptr<GLTFSceneNode> sceneNode, const tinygltf::Node &node, const tinygltf::Model &model);
                void RecalculateTangents(std::vector<Core::Vertex>& vertices, std::vector<uint32_t>& indices, uint64_t startIndex);

                void LoadAnimations(const tinygltf::Model &model);

                void LoadMaterials(std::shared_ptr<Resources::ResourceManager> pResMgr, const tinygltf::Model &model);
                void LoadTextures(std::shared_ptr<Resources::ResourceManager> pResMgr, const tinygltf::Model &model);
                std::shared_ptr<Core::CImage> LoadTexture(const tinygltf::Image &model, std::string path);
                void LoadSkins(const tinygltf::Model &model);

                uint32_t current_primitive;
                std::shared_ptr<Mesh::MeshBase> m_pMesh;
                //std::shared_ptr<Light::Point> m_pPointLights;
                //std::shared_ptr<Camera::Base> m_pCameras;
                std::vector<std::shared_ptr<Core::CImage>> vTextures;
                std::vector<std::shared_ptr<Material::MaterialBase>> vMaterials;

                std::vector<std::shared_ptr<GLTFSceneNode>> m_vNodes;
                std::vector<std::shared_ptr<GLTFSceneNode>> m_vLinearNodes;

                //Parameters
                bool bLoadTextures;
                bool bLoadMaterials;
                bool bUseMaterials;
                std::string srModelName;
                std::string srVolumeName;
            };
        }
    }
}