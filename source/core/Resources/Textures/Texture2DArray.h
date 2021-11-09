#pragma once
#include "VulkanTexture.h"

namespace Engine
{
    namespace Resources
    {
        namespace Texture
        {
            class Texture2DArray : public TextureBase
            {
            public:
                Texture2DArray() = default;
                Texture2DArray(std::shared_ptr<Core::Device> device);

                void ReCreate() override;
                void Update(uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
                void Destroy() override;
            };
        }
    }
}