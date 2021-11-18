#include "RendererBase.h"
#include "Resources/ResourceManager.h"
#include "Resources/Textures/ImageLoader.h"
#include "Resources/Textures/VulkanTexture.h"
#include "Resources/Materials/MaterialDeferred.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Rendering;
using namespace Engine::Resources;
using namespace Engine::Resources::Loaders;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;

// void FImageData::Destroy(std::shared_ptr<Device> device)
// {
//     device->Destroy(view);
//     device->Destroy(image);
//     device->Destroy(memory);
// }

// vk::DescriptorImageInfo FImageData::GetDescriptor(vk::Sampler& sampler)
// {
//     vk::DescriptorImageInfo descriptor{};
//     descriptor.imageView = view;
//     descriptor.imageLayout = layout;
//     descriptor.sampler = sampler;
//     return descriptor;
// }

RendererBase::~RendererBase()
{
    //Cleanup();
}


void RendererBase::Create(FRendererCreateInfo createInfo, std::shared_ptr<Resources::ResourceManager> pResMgr)
{
    auto framesInFlight = USwapChain->GetFramesInFlight();
    m_pUniform = std::make_shared<UniformBuffer>();
    m_pUniform->Create(framesInFlight, sizeof(FLightningData));

    m_vColorAttachments = std::move(createInfo.vColorAttachments);
    m_DepthAttachment = createInfo.depthAttachment;
    m_DepthAttachment.format = UDevice->GetDepthFormat();
    m_eRendererType = createInfo.eRendererType;

    CreateSampler();
    CreateImages();
    CreateRenderPass();
    CreateFramebuffers();
    CreateMaterial(pResMgr);
}

std::shared_ptr<vk::RenderPassBeginInfo> RendererBase::CreateRenderPassCI(uint32_t imageIndex)
{
    auto extent = USwapChain->GetExtent();
    std::vector<vk::ClearValue> clearValues{};
    for(auto& [attachment, param] : m_vColorAttachments)
    {
        vk::ClearValue clearValue{};
        clearValue.color = param.color;
        clearValues.emplace_back(clearValue);
    }

    //Clear depth
    vk::ClearValue clearValue{};
    clearValue.depthStencil = vk::ClearDepthStencilValue{1.f, 0};
    clearValues.emplace_back(clearValue);

    auto renderPassInfo = std::make_shared<vk::RenderPassBeginInfo>();
    renderPassInfo->renderPass = m_RenderPass;
    renderPassInfo->framebuffer = m_vFramebuffers.at(imageIndex);
    renderPassInfo->renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo->renderArea.extent = extent;
    renderPassInfo->clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo->pClearValues = clearValues.data();

    return renderPassInfo;
}

void RendererBase::ReCreate(uint32_t framesInFlight)
{
    CreateSampler();
    CreateImages();
    CreateRenderPass();
    CreateFramebuffers();
    m_pMaterial->ReCreate();
}

void RendererBase::Update(vk::CommandBuffer& commandBuffer, void* data)
{
    auto currentFrame = USwapChain->GetCurrentFrame();
    for(auto& [attachment, texture] : m_vImages.at(currentFrame))
        m_pMaterial->AddTexture(attachment, texture);
    
    m_pUniform->UpdateUniformBuffer(currentFrame, data);
    auto& buffer = m_pUniform->GetUniformBuffer(currentFrame);
    m_pMaterial->Update(buffer->GetDscriptor(), currentFrame);
    m_pMaterial->Bind(commandBuffer, currentFrame);
}

void RendererBase::Cleanup()
{
    /*for(uint32_t frame = 0; frame < m_iFramesInFlight; frame++)
    {
        auto currentFrame = m_vImages.at(frame);
        for(auto& [attachment, image] : currentFrame)
            image.Destroy(UDevice);
    }
    
    m_DepthImage.Destroy(UDevice);*/

    for (auto framebuffer : m_vFramebuffers)
        UDevice->Destroy(framebuffer);
    
    UDevice->Destroy(m_RenderPass);
    UDevice->Destroy(m_Sampler);

    m_vImages.clear();
}

void RendererBase::CreateSampler()
{
    UDevice->CreateSampler(m_Sampler, 1,  vk::SamplerAddressMode::eClampToEdge);
}

std::shared_ptr<TextureBase> RendererBase::CreateImage(attachment_t attachment)
{
    auto texture = std::make_shared<TextureBase>();
    auto extent = USwapChain->GetExtent();
    ktxTexture *offscreen;
    vk::Format imageFormat;
    ImageLoader::AllocateRawDataAsKTXTexture(&offscreen, &imageFormat, extent.width, extent.height, 1, 2, VulkanStaticHelper::VkFormatToGLFormat(attachment.format));

    vk::ImageAspectFlags aspectMask{};
    vk::ImageLayout imageLayout{};

    if(attachment.usage & vk::ImageUsageFlagBits::eColorAttachment)
    {
        aspectMask = vk::ImageAspectFlagBits::eColor;
        imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    }
    else if(attachment.usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
    {
        aspectMask = vk::ImageAspectFlagBits::eDepth;
        imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    }

    texture->SetSampler(m_Sampler);
    texture->InitializeTexture(offscreen, imageFormat, attachment.usage, aspectMask);
    if(attachment.usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
        texture->TransitionImageLayout(imageLayout, aspectMask, false);
    else
        texture->SetImageLayout(imageLayout);

    texture->UpdateDescriptor();
    return texture;
}

void RendererBase::CreateImages()
{
    auto framesInFlight = USwapChain->GetFramesInFlight();
    m_vImages.resize(framesInFlight);
    for(uint32_t frame = 0; frame < framesInFlight; frame++)
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
    // Set up separate renderpass with references to the color and depth attachments
	std::map<ETextureAttachmentType, vk::AttachmentDescription> descTemporary{};

    vk::AttachmentDescription desc{};
    desc.samples = vk::SampleCountFlagBits::e1;
	desc.loadOp = vk::AttachmentLoadOp::eClear;
	desc.storeOp = vk::AttachmentStoreOp::eStore;
	desc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	desc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    for(auto& [attachment, param] : m_vColorAttachments)
	{
        desc.format = param.format;
		desc.initialLayout = vk::ImageLayout::eUndefined;
		desc.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        descTemporary.emplace(attachment, desc);
	}

    //Depth source
    desc.format = m_DepthImage->GetParams().format;
    desc.initialLayout = vk::ImageLayout::eUndefined;
	desc.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    descTemporary.emplace(ETextureAttachmentType::eDepth, desc);

    std::vector<vk::AttachmentDescription> vAttachmentDesc{};
    std::transform(descTemporary.begin(), descTemporary.end(), std::back_inserter(vAttachmentDesc), get_map_values(descTemporary));

    std::vector<vk::AttachmentReference> colorReferences{};

    for(uint32_t i = 0; i < m_vColorAttachments.size(); i++)
        colorReferences.emplace_back(vk::AttachmentReference{i, vk::ImageLayout::eColorAttachmentOptimal});

    vk::AttachmentReference depthReference = {};
	depthReference.attachment = m_vColorAttachments.size();
	depthReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

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

    m_RenderPass = UDevice->GetLogical().createRenderPass(renderPassCI);
}

void RendererBase::CreateFramebuffers()
{
    assert(UDevice && "Cannot create framebuffers, cause logical device is not valid.");
    assert(m_RenderPass && "Cannot create framebuffers, cause render pass is not valid.");
    assert(!m_vImages.empty() && "Cannot create framebuffers, cause g-buffer images is not created.");

    auto framesInFlight = USwapChain->GetFramesInFlight();
    auto extent = USwapChain->GetExtent();
    m_vFramebuffers.resize(framesInFlight);
    for(size_t frame = 0; frame < framesInFlight; frame++)
    {
        std::vector<vk::ImageView> vImages{};
        auto& gbuffer = m_vImages.at(frame);

        for(auto& [attachment, texture] : gbuffer)
            vImages.push_back(texture->GetDescriptor().imageView);

        vImages.push_back(m_DepthImage->GetDescriptor().imageView);

        vk::FramebufferCreateInfo framebufferCI = {};
        framebufferCI.pNext = nullptr;
        framebufferCI.renderPass = m_RenderPass;
        framebufferCI.pAttachments = vImages.data();
        framebufferCI.attachmentCount = static_cast<uint32_t>(vImages.size());
        framebufferCI.width = extent.width;
        framebufferCI.height = extent.height;
        framebufferCI.layers = 1;

        m_vFramebuffers[frame] = UDevice->Make<vk::Framebuffer, vk::FramebufferCreateInfo>(framebufferCI);
    }
}

void RendererBase::CreateMaterial(std::shared_ptr<Resources::ResourceManager> pResMgr)
{
    assert(!m_vImages.empty() && "Composition material cannot be created, cause g-buffer images is not created.");

    m_pMaterial = std::make_shared<MaterialDeferred>();
    m_pMaterial->Create(nullptr);
    m_pMaterial->AddTexture(ETextureAttachmentType::eBRDFLUT, pResMgr->Get<TextureBase>("environment_component_brdf"));
    m_pMaterial->AddTexture(ETextureAttachmentType::eIrradiance, pResMgr->Get<TextureBase>("environment_component_irradiate_cube"));
    m_pMaterial->AddTexture(ETextureAttachmentType::ePrefiltred, pResMgr->Get<TextureBase>("environment_component_prefiltred_cube"));
}
