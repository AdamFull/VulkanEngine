#pragma once
#include "VulkanMaterial.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material
        {
            class MaterialSkybox : public MaterialBase
            {
            public:
                void Create() override;
                void ReCreate() override;
                void Update(uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;

            protected:
                Core::Pipeline::EShaderSet GetShaderSet() override { return Core::Pipeline::EShaderSet::eSkybox; }
                void CreateDescriptors(uint32_t images) override;
            };
        }
    }
}