#include "RendererBase.h"
#include "Resources/Textures/ImageLoader.h"
#include "Core/VulkanAllocator.h"

using namespace Engine::Core;
using namespace Engine::Core::Renderer;
using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;
using namespace Engine::Resources::Loaders;

vk::DescriptorImageInfo FImage::GetDescriptor()
{
    vk::DescriptorImageInfo desc{};
    desc.imageView = view;
    desc.imageLayout = layout;
    desc.sampler = sampler;
    return desc;
}

void RendererBase::Create()
{
    depthAttachment = 
    FAttachmentInfo
    (
        m_device->GetDepthFormat(),
        vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
        vk::ImageLayout::eDepthStencilAttachmentOptimal,
        vk::ImageAspectFlagBits::eDepth,
        {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
    );

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
        m_pMaterial->AddTexture(attachment, texture.GetDescriptor());
    
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


FImage RendererBase::CreateImage(vk::Format format, FAttachmentInfo attachment)
{
    auto extent = m_swapchain->GetExtent();
    FImage image{};

    vk::ImageCreateInfo imageInfo{};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = extent.width;
    imageInfo.extent.height = extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = attachment.usage;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.samples = vk::SampleCountFlagBits::e1;

    m_device->CreateImage(image.image, image.memory, imageInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = attachment.aspect;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    image.view = m_device->CreateImageView(image.image, viewInfo);

    image.sampler = sampler;

    if(attachment.usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
    {
        
    }

    return image;
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
            mBuffer.emplace(attachment, CreateImage(param.format, param));
        m_vImages[frame] = std::move(mBuffer);
    }

    m_DepthImage = CreateImage(depthAttachment.format, depthAttachment);
}

void RendererBase::CreateRenderPass()
{
    // Set up separate renderpass with references to the color and depth attachments
	std::map<ETextureAttachmentType, vk::AttachmentDescription> descTemporary{};

    vk::AttachmentDescription desc{};
    desc.samples = vk::SampleCountFlagBits::e1;
	desc.loadOp = vk::AttachmentLoadOp::eClear;
	desc.storeOp = vk::AttachmentStoreOp::eStore;
	desc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	desc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    for(auto& [attachment, param] : vAttachments)
	{
        desc.format = param.format;
		desc.initialLayout = vk::ImageLayout::eUndefined;
		desc.finalLayout = param.layout;
        descTemporary.emplace(attachment, desc);
	}

    //Depth source
    desc.format = depthAttachment.format;
    desc.initialLayout = vk::ImageLayout::eUndefined;
	desc.finalLayout = depthAttachment.layout;
    descTemporary.emplace(ETextureAttachmentType::eDepth, desc);

    std::vector<vk::AttachmentDescription> vAttachmentDesc{};
    std::transform(descTemporary.begin(), descTemporary.end(), std::back_inserter(vAttachmentDesc), get_map_values(descTemporary));

    std::vector<vk::AttachmentReference> colorReferences{};

    for(uint32_t i = 0; i < vAttachments.size(); i++)
        colorReferences.emplace_back(vk::AttachmentReference{i, vk::ImageLayout::eColorAttachmentOptimal});

    vk::AttachmentReference depthReference = {};
	depthReference.attachment = vAttachments.size();
	depthReference.layout = depthAttachment.layout;

    vk::SubpassDescription subpass = {};
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.pColorAttachments = colorReferences.data();
	subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
	subpass.pDepthStencilAttachment = &depthReference;

    // Use subpass dependencies for attachment layout transitions
	std::array<vk::SubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
	dependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
	dependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
	dependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
	dependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
	dependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
	dependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    vk::RenderPassCreateInfo renderPassCI = {};
	renderPassCI.pAttachments = vAttachmentDesc.data();
	renderPassCI.attachmentCount = static_cast<uint32_t>(vAttachmentDesc.size());
	renderPassCI.subpassCount = 1;
	renderPassCI.pSubpasses = &subpass;
	renderPassCI.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassCI.pDependencies = dependencies.data();

    renderPass = m_device->GetLogical().createRenderPass(renderPassCI);
}

void RendererBase::CreateFrameBuffers()
{
    assert(m_device && "Cannot create framebuffers, cause logical device is not valid.");
    assert(renderPass && "Cannot create framebuffers, cause render pass is not valid.");
    assert(!m_vImages.empty() && "Cannot create framebuffers, cause g-buffer images is not created.");

    auto framesInFlight = m_swapchain->GetFramesInFlight();
    auto extent = m_swapchain->GetExtent();

    m_vFramebuffers.resize(framesInFlight);
    for(size_t frame = 0; frame < framesInFlight; frame++)
    {
        std::vector<vk::ImageView> vImages{};
        auto& buffer = m_vImages.at(frame);

        for(auto& [attachment, texture] : buffer)
            vImages.push_back(texture.imageView);

        vImages.push_back(m_DepthImage.imageView);

        vk::FramebufferCreateInfo framebufferCI = {};
        framebufferCI.pNext = nullptr;
        framebufferCI.renderPass = renderPass;
        framebufferCI.pAttachments = vImages.data();
        framebufferCI.attachmentCount = static_cast<uint32_t>(vImages.size());
        framebufferCI.width = extent.width;
        framebufferCI.height = extent.height;
        framebufferCI.layers = 1;

        m_vFramebuffers[frame] = m_device->Make<vk::Framebuffer, vk::FramebufferCreateInfo>(framebufferCI);
    }
}

void RendererBase::CreateUniformBuffers()
{
    m_pUniform = FDefaultAllocator::Allocate<UniformBuffer>();
}

void RendererBase::CreateMaterial()
{
    assert(!m_vImages.empty() && "Composition material cannot be created, cause g-buffer images is not created.");
}
