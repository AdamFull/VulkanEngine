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
            VulkanVBO() = default;
            VulkanVBO(std::shared_ptr<Device> device);
            ~VulkanVBO();

            void Create();
            void Bind(vk::CommandBuffer commandBuffer);
            void AddMeshData(std::vector<Vertex> &&vertices, std::vector<uint32_t> &&indices);
            inline uint64_t GetLastIndex() { return m_vIndices.size(); }
            inline uint64_t GetLastVertex() { return m_vVertices.size(); }

        private:
            void CreateVertexBuffer();
            void CreateIndexBuffer();
            bool m_bBuffersCreated{false};

            std::vector<Vertex> m_vVertices;
            std::unique_ptr<VulkanBuffer> vertexBuffer;

            std::vector<uint32_t> m_vIndices;
            std::unique_ptr<VulkanBuffer> indexBuffer;

            std::shared_ptr<Device> m_device;
        };
    }
}