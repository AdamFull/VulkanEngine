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

                void Create(std::shared_ptr<ResourceManager> pResMgr, vk::RenderPass& rPass) override;
                void Create(std::shared_ptr<ResourceManager> pResMgr) override;
                void ReCreate() override;
                void Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
            };
        }
    }
}