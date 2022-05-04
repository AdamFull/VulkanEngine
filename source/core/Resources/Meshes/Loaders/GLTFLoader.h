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
        namespace Material { class CMaterialBase; }

        namespace Loaders
        {
            class GLTFSceneNode;
            
            struct GLTFLoader
            {
            public:
                GLTFLoader(bool loadMaterials, bool useMaterials, const std::string& modelName, const std::string& volumeName);

                void load(std::string srPath, std::string srName);

                inline void addMaterial(std::shared_ptr<Material::CMaterialBase> material) { vMaterials.emplace_back(material); }

                inline const std::shared_ptr<Mesh::CMeshBase> getMesh() const { return m_pMesh; }

            private:
                void loadNode(std::shared_ptr<GLTFSceneNode> pParent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, float globalscale);

                void loadMeshFragment(std::shared_ptr<GLTFSceneNode> sceneNode, const tinygltf::Node &node, const tinygltf::Model &model);
                void recalculateTangents(std::vector<Core::FVertex>& vertices, std::vector<uint32_t>& indices, uint64_t startIndex);

                void loadAnimations(const tinygltf::Model &model);

                void loadMaterials(const tinygltf::Model &model);
                void loadTextures(const tinygltf::Model &model);
                std::shared_ptr<Core::CImage> loadTexture(const tinygltf::Image &model, std::string path);
                void loadSkins(const tinygltf::Model &model);

                uint32_t current_primitive;
                std::shared_ptr<Mesh::CMeshBase> m_pMesh;
                //std::shared_ptr<Light::Point> m_pPointLights;
                //std::shared_ptr<Camera::Base> m_pCameras;
                std::vector<std::shared_ptr<Core::CImage>> vTextures;
                std::vector<std::shared_ptr<Material::CMaterialBase>> vMaterials;

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