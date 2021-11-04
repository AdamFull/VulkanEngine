#include "TextureCubemap.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanBuffer.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Resources::Texture;
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
