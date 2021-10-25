#include "TextureCubemap.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanHighLevel.h"

namespace Engine
{
    void TextureCubemap::ReCreate()
    {
        TextureBase::ReCreate();
    }

    void TextureCubemap::Update(uint32_t imageIndex)
    {
        TextureBase::Update(imageIndex);
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