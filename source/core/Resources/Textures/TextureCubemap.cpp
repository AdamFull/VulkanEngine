#include "TextureCubemap.h"

namespace Engine
{
    void TextureCubemap::ReCreate()
    {
        TextureBase::ReCreate();
    }

    void TextureCubemap::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        TextureBase::Update(imageIndex, pUniformBuffer);
    }

    void TextureCubemap::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        TextureBase::Bind(commandBuffer, imageIndex);
    }

    void TextureCubemap::Cleanup()
    {
        TextureBase::Cleanup();
    }

    void TextureCubemap::Destroy()
    {
        TextureBase::Destroy();
    }
}