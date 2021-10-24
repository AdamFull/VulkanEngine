#pragma once
#include "VulkanMaterial.h"

namespace Engine
{
    class MaterialDiffuse : public MaterialBase
    {
    public:
        void Create() override;
        void ReCreate() override;
        void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;
    protected:
        EShaderSet GetShaderSet() override { return EShaderSet::eDiffuse; }
        void CreateDescriptorSetLayout() override;
        void CreateDescriptorSets(uint32_t images) override;
    };
}