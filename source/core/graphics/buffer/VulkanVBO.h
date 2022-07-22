#pragma once
#include "graphics/data_types/VulkanVertex.hpp"
#include "VulkanBuffer.h"

namespace engine
{
    namespace core
    {
        class CVertexBufferObject : public utl::non_copy_movable
        {
        public:
            CVertexBufferObject() = default;
            ~CVertexBufferObject();

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
            utl::scope_ptr<CVulkanBuffer> vertexBuffer;

            std::vector<uint32_t> vIndices;
            utl::scope_ptr<CVulkanBuffer> indexBuffer;
        };
    }
}