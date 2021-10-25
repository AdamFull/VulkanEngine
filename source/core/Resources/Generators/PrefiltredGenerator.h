#pragma once
#include "VulkanGenerator.h"

namespace Engine
{
    class PrefiltredGenerator : public GeneratorBase
    {
    public:
        void Create() override;
        void ReCreate() override;
        void Update(uint32_t imageIndex) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;

    protected:
        inline EShaderSet GetShaderSet() override { return EShaderSet::eNone; }
        void CreateRenderPass() override;
        void CreateFramebuffer() override;
        void CreateDescriptorSets(uint32_t images) override;
    };
}