#include "Texture2D.h"

namespace Engine
{
    void Texture2D::ReCreate()
    {
        TextureBase::ReCreate();
    }

    void Texture2D::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        TextureBase::Update(imageIndex, pUniformBuffer);
    }

    void Texture2D::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        TextureBase::Bind(commandBuffer, imageIndex);
    }

    void Texture2D::Cleanup()
    {
        TextureBase::Cleanup();
    }

    void Texture2D::Destroy()
    {
        TextureBase::Destroy();
    }
}