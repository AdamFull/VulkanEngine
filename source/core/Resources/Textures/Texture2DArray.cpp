#include "Texture2DArray.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Resources::Texture;

Texture2DArray::Texture2DArray(std::shared_ptr<Core::Device> device)
{
    m_device = device;
}

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
