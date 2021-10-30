#pragma once
#include "VulkanGenerator.h"

namespace Engine
{
    namespace Resources
    {
        namespace Generators
        {
            class BRDFLUTGenerator : public GeneratorBase
            {
            public:
                void Create() override;
                void ReCreate() override;
                void Update(uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
                void Destroy() override;

            protected:
                inline Core::Pipeline::EShaderSet GetShaderSet() override { return Core::Pipeline::EShaderSet::eNone; }
                void CreateRenderPass() override;
                void CreateFramebuffer() override;
                void CreateDescriptorSets(uint32_t images) override;
            };
        }
    }
}