#pragma once
#include "external/tinygltf/tiny_gltf.h"
#include "resources/meshes/VulkanMesh.h"
#include "graphics/data_types/VulkanVertex.hpp"

namespace engine
{
    namespace core { class CImage; }
    namespace resources
    {
        namespace material { class CMaterialBase; }

        namespace loaders
        {
            class GLTFSceneNode;
            
            /**
             * @brief GLTF model loader
             * 
             */
            struct GLTFLoader
            {
            public:
            /**
             * @brief Construct a new GLTFLoader object
             * 
             * @param loadMaterials Load included materials or not
             * @param useMaterials Use included materials or load empty
             * @param modelName Model name
             * @param volumeName Model volume name (deprecated)
             */
                GLTFLoader(bool loadMaterials, bool useMaterials, const std::string& modelName, const std::string& volumeName);

                /**
                 * @brief Load model from file
                 * 
                 * @param srPath Path to model file
                 * @param srName Model name
                 */
                void load(const std::string& srPath, const std::string& srName);

                /**
                 * @brief Add new material
                 * 
                 * @param material Material smart pointer object
                 */
                inline void addMaterial(ref_ptr<material::CMaterialBase>&& material) { vMaterials.emplace_back(material); }

                /**
                 * @brief Get loaded mesh
                 * 
                 * @return const ref_ptr<Mesh::CMeshBase> Loaded mesh smart pointer object
                 */
                inline const ref_ptr<mesh::CMeshBase> getMesh() const { return m_pMesh; }

            private:
                /**
                 * @brief Loads gltf scene node
                 * 
                 * @param pParent Parent node
                 * @param node Current scene node
                 * @param nodeIndex Current node index
                 * @param model Current node mesh data
                 * @param globalscale Override global node scale
                 */
                void loadNode(ref_ptr<GLTFSceneNode> pParent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, float globalscale);

                /**
                 * @brief Loads mesh fragment from scene node
                 * 
                 * @param sceneNode Current node
                 * @param node Current scene node
                 * @param model Current node mesh data
                 */
                void loadMeshFragment(ref_ptr<GLTFSceneNode>& sceneNode, const tinygltf::Node &node, const tinygltf::Model &model);

                /**
                 * @brief Recalculates tangents for fragment vertices and indices
                 * 
                 * @param vertices Input vertex data
                 * @param indices Input index data
                 * @param startIndex Index in global VBO
                 */
                void recalculateTangents(std::vector<core::FVertex>& vertices, std::vector<uint32_t>& indices, uint64_t startIndex);

                /**
                 * @brief Loads mesh attached animations
                 * 
                 * @param model Current node mesh data
                 */
                void loadAnimations(const tinygltf::Model &model);

                /**
                 * @brief Load attached materials
                 * 
                 * @param model Current node mesh data
                 */
                void loadMaterials(const tinygltf::Model &model);

                /**
                 * @brief Load attached textures
                 * 
                 * @param model Current node mesh data
                 */
                void loadTextures(const tinygltf::Model &model);

                /**
                 * @brief Load single texture helper
                 * 
                 * @param model Current node mesh data
                 * @param path Texture path
                 * @return ref_ptr<Core::CImage> Smart pointer texture object
                 */
                ref_ptr<core::CImage> loadTexture(const tinygltf::Image &model, const fs::path& path);

                /**
                 * @brief Load mesh skinning data
                 * 
                 * @param model Current node mesh data
                 */
                void loadSkins(const tinygltf::Model &model);

                uint32_t current_primitive;
                ref_ptr<mesh::CMeshBase> m_pMesh;
                //ref_ptr<Light::Point> m_pPointLights;
                //ref_ptr<Camera::Base> m_pCameras;
                std::vector<ref_ptr<core::CImage>> vTextures;
                std::vector<ref_ptr<material::CMaterialBase>> vMaterials;

                std::vector<ref_ptr<GLTFSceneNode>> m_vNodes;
                std::vector<ref_ptr<GLTFSceneNode>> m_vLinearNodes;

                fs::path fsParentPath{""};

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