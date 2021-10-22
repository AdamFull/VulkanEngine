#pragma once
#include "Resources/ResourceBase.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/DataTypes/VulkanVertex.hpp"

namespace Engine
{
    class Device;
    class MaterialBase;

    struct Primitive
    {
        uint32_t firstIndex;
		uint32_t indexCount;
		uint32_t firstVertex;
		uint32_t vertexCount;
		std::shared_ptr<MaterialBase> material;
    };

    class MeshBase : public ResourceBase
    {
    public:
        virtual void Create();
        virtual void AddPrimitive(std::string srPrimitiveName, Primitive&& primitive);
        virtual void SetMaterial(std::string srPrimitiveName, std::shared_ptr<MaterialBase> material);
        void ReCreate() override;
        void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;
    protected:
        std::map<std::string, Primitive> m_mPrimitives;
    };
}