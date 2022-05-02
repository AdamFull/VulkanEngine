#pragma once
#include "VulkanMaterial.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material
        {
            class CMaterialShadow : public CMaterialBase
            {
            public:
                CMaterialShadow() = default;

                void create(vk::RenderPass& renderPass, uint32_t subpass) override;
                void reCreate() override;
                void update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex) override;
                void bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void cleanup() override;
            };
        }
    }
}