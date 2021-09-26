#pragma once
#include "VulkanVertex.h"

namespace Engine
{
    class Device;

    class VulkanMeshBase
    {
    public:
        std::vector<Vertex> vertices;
        vk::Buffer vertexBuffer;
        vk::DeviceMemory vertexBufferMemory;

        std::vector<uint32_t> indices;
        vk::Buffer indexBuffer;
        vk::DeviceMemory indiciesBufferMemory;
    };

    class VulkanStaticMesh : public VulkanMeshBase
    {
    public:
        void LoadStaticMesh(std::unique_ptr<Device>& device, std::string srPath, FTransform transform);
        
        void Bind(vk::CommandBuffer& commandBuffer);
        void Draw(vk::CommandBuffer& commandBuffer);
    private:
        void CreateVertexBuffer(std::unique_ptr<Device>& device);
        void CreateIndexBuffer(std::unique_ptr<Device>& device);
    };
}