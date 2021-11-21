#include "Image2D.h"

using namespace Engine::Resources::Texture;

void Image2D::ReCreate()
{
    Image::ReCreate();
}

void Image2D::Update(uint32_t imageIndex)
{
    Image::Update(imageIndex);
}

void Image2D::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    Image::Bind(commandBuffer, imageIndex);
}

void Image2D::Cleanup()
{
    Image::Cleanup();
}

void Image2D::Destroy()
{
    Image::Destroy();
}
