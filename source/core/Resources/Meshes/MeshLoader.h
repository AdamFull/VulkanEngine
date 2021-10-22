#pragma once
#include "Renderer/DataTypes/VulkanVertex.hpp"

namespace Engine
{
    class MeshBase;
    class ResourceManager;

    struct MeshLoader
    {
    public:
        static bool Load(std::string srPath, std::shared_ptr<ResourceManager> pResourceManager, std::shared_ptr<MeshBase> pMesh, bool bLoadMaterial = false);
    private:
        static void CalculateTangents(std::vector<Vertex>& vertices, std::vector<uint32_t> indices);
        static glm::vec4 GenerateNormals(std::vector<Vertex>& vertices, std::vector<uint32_t> indices);
    };
}