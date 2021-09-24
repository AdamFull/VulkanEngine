#pragma once
#include "VulkanVertex.h"

namespace Engine
{
    class VulkanMeshBase
    {
    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    class VulkanStaticMesh : public VulkanMeshBase
    {
    public:
        void LoadStaticMesh(std::string srPath, VulkanTransform transform);
    };
}