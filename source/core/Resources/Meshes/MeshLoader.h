#pragma once
#include "resources/ResourceManager.h"
#include "resources/meshes/MeshFragment.h"
#include "graphics/data_types/VulkanVertex.hpp"

namespace Engine
{
    namespace Resources
    {
        namespace Loaders
        {
            /**
             * @brief Mesh loader class. Loads mesh from file. Not working now. Supported only gltf models
             * 
             */
            struct CMeshLoader
            {
            public:
                /**
                 * @brief Loads mesh from file
                 * 
                 * @param srPath Path to model object
                 * @param pMesh Empty mesh object to fill
                 * @param bLoadMaterial Use included material
                 * @return true when model was loaded successfully
                 */
                static bool load(const std::string& srPath, ref_ptr<Mesh::CMeshFragment> pMesh, bool bLoadMaterial = false);

            private:
                /**
                 * @brief Calculate tangent vectors
                 * 
                 * @param vertices Input vertices
                 * @param indices Input Indices
                 */
                static void calculateTangents(std::vector<Core::FVertex> &vertices, std::vector<uint32_t> indices);

                /**
                 * @brief Calculate normals for mesh
                 * 
                 * @param vertices Input vertices
                 * @param indices Input Indices
                 * @return glm::vec4 Output normal vector
                 */
                static glm::vec4 generateNormals(std::vector<Core::FVertex> &vertices, std::vector<uint32_t> indices);
            };
        }
    }
}