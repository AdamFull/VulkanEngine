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
            struct CMeshLoader
            {
            public:
                static bool load(std::string srPath, std::shared_ptr<Resources::CResourceManager> pResourceManager, std::shared_ptr<Mesh::CMeshFragment> pMesh, bool bLoadMaterial = false);

            private:
                static void calculateTangents(std::vector<Core::FVertex> &vertices, std::vector<uint32_t> indices);
                static glm::vec4 generateNormals(std::vector<Core::FVertex> &vertices, std::vector<uint32_t> indices);
            };
        }
    }
}