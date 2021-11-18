#pragma once
#include "VulkanTexture.h"

namespace Engine
{
    namespace Resources
    {
        namespace Texture
        {
            class Texture2D : public TextureBase
            {
            public:
                Texture2D() = default;

                void ReCreate() override;
                void Update(uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
                void Destroy() override;
            };
        }
    }
}