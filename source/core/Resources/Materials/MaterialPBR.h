#pragma once
#include "VulkanMaterial.h"

namespace Engine
{
    class MaterialPBR : public MaterialBase
    {
    public:
        void Create(std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void ReCreate() override;
        void Update(uint32_t imageIndex) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;
    protected:
        EShaderSet GetShaderSet() override { return EShaderSet::ePBR; }
        void CreateDescriptors(uint32_t images, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
    };
}