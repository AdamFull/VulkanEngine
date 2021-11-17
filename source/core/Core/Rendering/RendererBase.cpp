#include "RendererBase.h"
#include "Core/VulkanAllocator.h"

using namespace Engine::Core;
using namespace Engine::Core::Rendering;
using namespace Engine::Resources::Material;

void FImageData::Destroy(std::shared_ptr<Device> device)
{
    device->Destroy(view);
    device->Destroy(image);
    device->Destroy(memory);
}

vk::DescriptorImageInfo FImageData::GetDescriptor(vk::Sampler& sampler)
{
    vk::DescriptorImageInfo descriptor{};
    descriptor.imageView = view;
    descriptor.imageLayout = layout;
    descriptor.sampler = sampler;
    return descriptor;
}


RendererBase::RendererBase(std::shared_ptr<Device> device)
{
    m_device = device;
}

RendererBase::~RendererBase()
{
    Cleanup();
}


void RendererBase::Create(FRendererCreateInfo createInfo)
{
    auto depthFormat = m_device->GetDepthFormat();
}

void RendererBase::ReCreate(uint32_t framesInFlight)
{
    m_pMaterial->ReCreate();
}

void RendererBase::Update(vk::CommandBuffer& commandBuffer, void* data)
{

}

void RendererBase::Cleanup()
{
    for(auto& [attachment, image] : m_vImages)
        image.Destroy(m_device);
    
    m_DepthImage.Destroy(m_device);

    for (auto framebuffer : m_vFramebuffers)
        m_device->Destroy(framebuffer);
    
    m_device->Destroy(m_RenderPass);
    m_device->Destroy(m_Sampler);

    m_vImages.clear();
}

void RendererBase::CreateSampler()
{
    m_device->CreateSampler(m_Sampler, 1,  vk::SamplerAddressMode::eClampToEdge);
}

FImageData RendererBase::CreateImage(attachment_t attachment)
{

}

void RendererBase::CreateImages()
{
    m_vImages.resize(m_iFramesInFlight);
    for(uint32_t frame = 0; frame < m_iFramesInFlight; frame++)
    {
        image_map_t images;
        for(auto& [attachment, param] : m_vColorAttachments)
            images.emplace(attachment, CreateImage(param));
        m_vImages[frame] = std::move(images);
    }

    m_DepthImage = CreateImage(m_DepthAttachment);
}

void RendererBase::CreateRenderPass()
{

}

void RendererBase::CreateFramebuffers()
{

}

void RendererBase::CreateMaterial()
{

}
