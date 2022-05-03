#pragma once
#include "graphics/data_types/VulkanVertex.hpp"

namespace Engine
{
    namespace Core
    {
        class CVulkanBuffer;
        class CVulkanVBO : public utl::non_copy_movable
        {
        public:
            CVulkanVBO() = default;
            ~CVulkanVBO();

            void create();
            void bind(vk::CommandBuffer commandBuffer);
            void addMeshData(std::vector<FVertex> &&vertices, std::vector<uint32_t> &&indices);
            inline uint64_t getLastIndex() { return vIndices.size(); }
            inline uint64_t getLastVertex() { return vVertices.size(); }

        private:
            void createVertexBuffer();
            void createIndexBuffer();
            bool bBuffersCreated{false};

            std::vector<FVertex> vVertices;
            std::unique_ptr<CVulkanBuffer> vertexBuffer;

            std::vector<uint32_t> vIndices;
            std::unique_ptr<CVulkanBuffer> indexBuffer;
        };
    }
}