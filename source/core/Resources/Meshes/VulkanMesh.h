#pragma once
#include "Resources/ResourceBase.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/DataTypes/VulkanVertex.hpp"

namespace Engine
{
    class Device;
    class MaterialBase;

    class MeshBase : public ResourceBase
    {
    public:
        void Create(std::string srResourcePath);
        virtual void AddMaterial(std::shared_ptr<MaterialBase> material);
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

        std::vector<std::shared_ptr<MaterialBase>> vMaterials;
    };
}