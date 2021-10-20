#pragma once
#include "Renderer/DataTypes/VulkanVertex.hpp"

namespace Engine
{
    struct MeshLoader
    {
    public:
        static bool Load(std::string srPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool bLoadMaterial = false);
    private:
        static bool LoadObj(std::string srPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool bLoadMaterial = false);
        static bool LoadNif(std::string srPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool bLoadMaterial = false);
        static bool LoadFbx(std::string srPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool bLoadMaterial = false);
        static bool LoadStl(std::string srPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool bLoadMaterial = false);
        static bool LoadGltf(std::string srPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool bLoadMaterial = false);
        static void CalculateTangents(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    };
}