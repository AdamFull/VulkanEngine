#include "VulkanGenerator.h"
#include "Resources/Textures/VulkanTexture.h"

using namespace Engine::Resources::Generators;

void GeneratorBase::Create()
{
    m_pGenerated = std::make_shared<Texture::TextureBase>();
    m_pFramebufferImage = std::make_shared<Texture::TextureBase>();
}

void GeneratorBase::ReCreate()
{
}

void GeneratorBase::Update(uint32_t imageIndex)
{
    ResourceBase::Update(imageIndex);
}

void GeneratorBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    ResourceBase::Bind(commandBuffer, imageIndex);
}

void GeneratorBase::Cleanup()
{
    ResourceBase::Cleanup();
}

void GeneratorBase::Destroy()
{
    ResourceBase::Destroy();
}

void GeneratorBase::CreateRenderPass()
{
}

void GeneratorBase::CreateFramebuffer()
{
}

void GeneratorBase::CreateDescriptorSets(uint32_t images)
{
}