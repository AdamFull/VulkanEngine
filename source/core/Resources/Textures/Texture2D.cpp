#include "Texture2D.h"

using namespace Engine::Resources::Texture;

Texture2D::Texture2D(std::shared_ptr<Core::Device> device)
{
    m_device = device;
}

void Texture2D::ReCreate()
{
    TextureBase::ReCreate();
}

void Texture2D::Update(uint32_t imageIndex)
{
    TextureBase::Update(imageIndex);
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
