#pragma once
#include "Core/DataTypes/VulkanVertex.hpp"

namespace Engine
{
    namespace Core
    {
        class VulkanBuffer;
        class Device;
        class VulkanVBO
        {
        public:
            void Create(std::unique_ptr<Device> &device);
            void Bind(vk::CommandBuffer commandBuffer);
            void Destroy(std::unique_ptr<Device> &device);
            void AddMeshData(std::vector<Vertex> &&vertices, std::vector<uint32_t> &&indices);
            inline uint64_t GetLastIndex() { return m_vIndices.size(); }
            inline uint64_t GetLastVertex() { return m_vVertices.size(); }

        private:
            void CreateVertexBuffer(std::unique_ptr<Device> &device);
            void CreateIndexBuffer(std::unique_ptr<Device> &device);
            bool m_bBuffersCreated{false};

            std::vector<Vertex> m_vVertices;
            std::unique_ptr<VulkanBuffer> vertexBuffer;

            std::vector<uint32_t> m_vIndices;
            std::unique_ptr<VulkanBuffer> indexBuffer;
        };
    }
}