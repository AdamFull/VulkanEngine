#include "RendererBase.h"
#include "Resources/Textures/ImageLoader.h"
#include "Core/VulkanAllocator.h"

using namespace Engine::Core;
using namespace Engine::Core::Renderer;
using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;
using namespace Engine::Resources::Loaders;

void RendererBase::Create()
{
    CreateSampler();
    CreateImages();
    CreateRenderPass();
    CreateFrameBuffers();
    CreateUniformBuffers();
    CreateMaterial();
}

void RendererBase::ReCreate()
{
    CreateSampler();
    CreateImages();
    CreateRenderPass();
    CreateFrameBuffers();
    m_pUniform->ReCreate(m_swapchain->GetFramesInFlight());
    m_pMaterial->ReCreate();
}

void RendererBase::Update(vk::CommandBuffer& commandBuffer, void* ubo)
{
    auto currentFrame = m_swapchain->GetCurrentFrame();
    for(auto& [attachment, texture] : m_vImages.at(currentFrame))
        m_pMaterial->AddTexture(attachment, texture);
    
    m_pUniform->UpdateUniformBuffer(currentFrame, ubo);
    auto& buffer = m_pUniform->GetUniformBuffer(currentFrame);
    m_pMaterial->Update(buffer->GetDscriptor(), currentFrame);
    m_pMaterial->Bind(commandBuffer, currentFrame);
}

void RendererBase::Cleanup()
{
    for (auto framebuffer : m_vFramebuffers)
        m_device->Destroy(framebuffer);

    m_device->Destroy(renderPass);

    m_vImages.clear();
    m_pDepthImage->Cleanup();
    m_pMaterial->Cleanup();
    m_pUniform->Cleanup();

    m_device->Destroy(sampler);
}


std::shared_ptr<TextureBase> RendererBase::CreateImage(vk::Format format, vk::ImageUsageFlags usage)
{
    auto extent = m_swapchain->GetExtent();
    auto texture = FDefaultAllocator::Allocate<TextureBase>();
    ktxTexture *offscreen;
    vk::Format imageFormat;
    ImageLoader::AllocateRawDataAsKTXTexture(&offscreen, &imageFormat, extent.width, extent.height, 1, 2, VulkanStaticHelper::VkFormatToGLFormat(format));

    vk::ImageAspectFlags aspectMask{};
    vk::ImageLayout imageLayout{};

    if(usage & vk::ImageUsageFlagBits::eColorAttachment)
    {
        aspectMask = vk::ImageAspectFlagBits::eColor;
        imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    }
    else if(usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
    {
        aspectMask = vk::ImageAspectFlagBits::eDepth/* | vk::ImageAspectFlagBits::eStencil*/;
        imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    }

    texture->SetSampler(sampler);
    texture->InitializeTexture(offscreen, imageFormat, usage, aspectMask);
    if(usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
        texture->TransitionImageLayout(imageLayout, aspectMask, false);
    else
        texture->SetImageLayout(imageLayout);

    texture->UpdateDescriptor();
    return texture;
}

void RendererBase::CreateSampler()
{
    m_device->CreateSampler(sampler, 1,  vk::SamplerAddressMode::eClampToEdge);
}

void RendererBase::CreateImages()
{
    auto framesInFlight = m_swapchain->GetFramesInFlight();
    m_vImages.resize(framesInFlight);
    for(uint32_t frame = 0; frame < framesInFlight; frame++)
    {
        gbuffer_t mBuffer;
        for(auto& [attachment, param] : vAttachments)
            mBuffer.emplace(attachment, CreateImage(param.format, param.usage));
        m_vImages[frame] = std::move(mBuffer);
    }

    //TODO: attach depth settings
    auto depthFormat = m_device->GetDepthFormat();
    m_pDepthImage = CreateImage(depthFormat, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled);
}

void RendererBase::CreateRenderPass()
{

}

void RendererBase::CreateFrameBuffers()
{
    assert(m_device && "Cannot create framebuffers, cause logical device is not valid.");
    assert(renderPass && "Cannot create framebuffers, cause render pass is not valid.");
    assert(!m_vImages.empty() && "Cannot create framebuffers, cause g-buffer images is not created.");
}

void RendererBase::CreateUniformBuffers()
{
    m_pUniform = FDefaultAllocator::Allocate<UniformBuffer>();
}

void RendererBase::CreateMaterial()
{
    assert(!m_vImages.empty() && "Composition material cannot be created, cause g-buffer images is not created.");
}
