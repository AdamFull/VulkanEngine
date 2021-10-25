#include "Texture2DArray.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanHighLevel.h"

namespace Engine
{
    void Texture2DArray::ReCreate()
    {
        TextureBase::ReCreate();
    }

    void Texture2DArray::Update(uint32_t imageIndex)
    {
        TextureBase::Update(imageIndex);
    }

    void Texture2DArray::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        TextureBase::Bind(commandBuffer, imageIndex);
    }

    void Texture2DArray::Cleanup()
    {
        TextureBase::Cleanup();
    }

    void Texture2DArray::Destroy()
    {
        TextureBase::Destroy();
    }
}