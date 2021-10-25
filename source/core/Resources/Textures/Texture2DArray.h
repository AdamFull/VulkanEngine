#pragma once
#include "VulkanTexture.h"

namespace Engine
{
    class Texture2DArray : public TextureBase
    {
    public:
        void ReCreate() override;
        void Update(uint32_t imageIndex) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;
    protected:
        uint32_t GetInternalFormat() override { return 0x8C43; } //GL_SRGB8_ALPHA8, for texture generators
    };
}