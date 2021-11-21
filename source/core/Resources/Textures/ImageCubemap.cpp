#include "ImageCubemap.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanBuffer.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Resources::Texture;

void ImageCubemap::ReCreate()
{
    Image::ReCreate();
}

void ImageCubemap::Update(uint32_t imageIndex)
{
    Image::Update(imageIndex);
}

void ImageCubemap::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    Image::Bind(commandBuffer, imageIndex);
}

void ImageCubemap::Cleanup()
{
    Image::Cleanup();
}

void ImageCubemap::Destroy()
{
    Image::Destroy();
}
