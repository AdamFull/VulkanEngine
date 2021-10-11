#pragma once
#include "VulkanTexture.h"

namespace Engine
{
    class Texture2D : public TextureBase
    {
    public:
        void ReCreate() override;
        void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;
    };
}