#include "Image2DArray.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Resources::Texture;

void Image2DArray::ReCreate()
{
    Image::ReCreate();
}

void Image2DArray::Update(uint32_t imageIndex)
{
    Image::Update(imageIndex);
}

void Image2DArray::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    Image::Bind(commandBuffer, imageIndex);
}

void Image2DArray::Cleanup()
{
    Image::Cleanup();
}

void Image2DArray::Destroy()
{
    Image::Destroy();
}
