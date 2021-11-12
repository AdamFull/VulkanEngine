#pragma once
#include "external/gltf/tiny_gltf.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;

        namespace Texture { class TextureBase; }
        namespace Material { class MaterialBase; }
        namespace Mesh { class MeshBase; }

        namespace Loaders
        {
            class GLTFSceneNode;
            
            struct GLTFLoader
            {
            public:
                GLTFLoader(bool loadMaterials, const std::string& modelName);

                void Load(std::string srPath, std::string srName, std::shared_ptr<Resources::ResourceManager> pResMgr);

                inline void AddMaterial(std::shared_ptr<Material::MaterialBase> material) { vMaterials.emplace_back(material); }

                inline std::shared_ptr<Mesh::MeshBase> GetMesh() { return m_pMesh; }

            private:
                void LoadNode(std::shared_ptr<Resources::ResourceManager> pResMgr, std::shared_ptr<GLTFSceneNode> pParent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, float globalscale);

                void LoadMeshFragment(std::shared_ptr<Resources::ResourceManager> pResMgr, std::shared_ptr<GLTFSceneNode> sceneNode, const tinygltf::Node &node, const tinygltf::Model &model);

                void LoadMaterials(std::shared_ptr<Resources::ResourceManager> pResMgr, const tinygltf::Model &model);
                void LoadTextures(std::shared_ptr<Resources::ResourceManager> pResMgr, const tinygltf::Model &model);
                std::shared_ptr<Texture::TextureBase> LoadTexture(const tinygltf::Image &model, std::string path);
                void LoadSkins(const tinygltf::Model &model);

                uint32_t current_primitive;
                std::shared_ptr<Mesh::MeshBase> m_pMesh;
                //std::shared_ptr<Light::Point> m_pPointLights;
                //std::shared_ptr<Camera::Base> m_pCameras;
                std::vector<std::shared_ptr<Texture::TextureBase>> vTextures;
                std::vector<std::shared_ptr<Material::MaterialBase>> vMaterials;

                std::vector<std::shared_ptr<GLTFSceneNode>> m_vNodes;
                std::vector<std::shared_ptr<GLTFSceneNode>> m_vLinearNodes;

                //Parameters
                bool bLoadTextures;
                bool bLoadMaterials;
                std::string srModelName;
                std::string srVolumeName;
            };
        }
    }
}