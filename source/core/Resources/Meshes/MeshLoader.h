#pragma once
#include "Core/DataTypes/VulkanVertex.hpp"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;
        namespace Mesh { class MeshFragment; }
        
        namespace Loaders
        {
            struct MeshLoader
            {
            public:
                static bool Load(std::string srPath, std::shared_ptr<Resources::ResourceManager> pResourceManager, std::shared_ptr<Mesh::MeshFragment> pMesh, bool bLoadMaterial = false);

            private:
                static void CalculateTangents(std::vector<Core::Vertex> &vertices, std::vector<uint32_t> indices);
                static glm::vec4 GenerateNormals(std::vector<Core::Vertex> &vertices, std::vector<uint32_t> indices);
            };
        }
    }
}