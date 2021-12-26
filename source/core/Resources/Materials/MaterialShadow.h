#pragma once
#include "VulkanMaterial.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material
        {
            class MaterialShadow : public MaterialBase
            {
            public:
                MaterialShadow() = default;

                void Create(std::shared_ptr<ResourceManager> pResMgr) override;
                void ReCreate() override;
                void Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;

            protected:
                Core::Pipeline::EShaderSet GetShaderSet() override { return Core::Pipeline::EShaderSet::eShadow; }
                void CreateDescriptors(uint32_t images) override;
            };
        }
    }
}