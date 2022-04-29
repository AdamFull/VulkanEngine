#pragma once
#include "VulkanMaterial.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material
        {
            class MaterialBlur : public MaterialBase
            {
            public:
                MaterialBlur() = default;

                void Create(vk::RenderPass& renderPass, uint32_t subpass) override;
                void ReCreate() override;
                void Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
            };
        }
    }
}