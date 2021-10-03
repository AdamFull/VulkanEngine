#pragma once
#include "Resources/ResourceBase.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/DataTypes/VulkanVertex.hpp"

namespace Engine
{
    class Device;

    class MeshBase : public ResourceBase
    {
    public:
        void Create(std::string srResourcePath);
        void ReCreate() override;
        void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;
    protected:
        virtual void CreateVertexBuffer();
        virtual void CreateIndexBuffer();

        std::vector<Vertex> vertices;
        std::unique_ptr<VulkanBuffer> vertexBuffer;

        std::vector<uint32_t> indices;
        std::unique_ptr<VulkanBuffer> indexBuffer;
    };

    class StaticMesh : public MeshBase
    {
    public:
        void Create(std::string srResourcePath);
        void ReCreate() override;
        void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;

        void Load(std::string srPath);
        
    };
}