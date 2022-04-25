#pragma once
#include "graphics/data_types/VulkanVertex.hpp"

namespace Engine
{
    namespace Core
    {
        class VulkanBuffer;
        
        class VulkanVBO : public utl::non_copy_movable
        {
        public:
            VulkanVBO() = default;
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
        };
    }
}