#include "RendererBase.h"
#include "Resources/ResourceManager.h"
#include "Resources/Textures/ImageLoader.h"
#include "Resources/Textures/Image.h"
#include "Core/VulkanHighLevel.h"
#include "Core/VulkanInitializers.h"

using namespace Engine::Core;
using namespace Engine::Core::Rendering;
using namespace Engine::Resources;
using namespace Engine::Resources::Loaders;
using namespace Engine::Resources::Texture;

RendererBase::~RendererBase()
{
    //Cleanup();
}

void RendererBase::Create(std::shared_ptr<Resources::ResourceManager> pResMgr)
{
    m_DepthAttachment = 
    FRendererCreateInfo::FAttachmentInfo
    (
        UDevice->GetDepthFormat(),
        vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
        {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
    );

    CreateSampler();
    CreateImages();
    CreateRenderPass();
    CreateFramebuffers();

    if(m_pNext) m_pNext->Create(pResMgr);
}

void RendererBase::Render(vk::CommandBuffer& commandBuffer)
{
    if(m_pNext) m_pNext->Render(commandBuffer);
}

void RendererBase::ReCreate(uint32_t framesInFlight)
{
    CreateSampler();
    CreateImages();
    CreateRenderPass();
    CreateFramebuffers();
    if(m_pNext) m_pNext->ReCreate(framesInFlight);
}

void RendererBase::Cleanup()
{
    m_vImages.clear();
    if(m_pNext) m_pNext->Cleanup();
}

std::shared_ptr<RendererBase> RendererBase::Find(FRendererCreateInfo::ERendererType eType)
{
    if(eType == m_eRendererType)
        return shared_from_this();
    
    if(m_pNext) return m_pNext->Find(eType);
    return nullptr;
}

void RendererBase::SetNextStage(std::shared_ptr<RendererBase> pNext)
{
    if(!m_pNext)
    {
        m_pNext = pNext;
        m_pNext->SetPrevStage(shared_from_this());
        return;
    }
    m_pNext->SetNextStage(pNext);
}

void RendererBase::SetPrevStage(std::shared_ptr<RendererBase> pPrev)
{
    m_pPrev = pPrev;
}

void RendererBase::BeginRender(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = USwapChain->GetCurrentFrame();

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

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.renderPass = GetRenderPass();
    renderPassInfo.framebuffer = GetFramebuffer(imageIndex);
    renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo.renderArea.extent = out_extent;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport = Initializers::Viewport(out_extent.width, out_extent.height);
    vk::Rect2D scissor{{0, 0}, out_extent};

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);
}

void RendererBase::EndRender(vk::CommandBuffer& commandBuffer)
{
    commandBuffer.endRenderPass();
}

std::shared_ptr<Image> RendererBase::GetProduct(texture_type_t eType)
{
    auto mProductMap = GetProducts();
    return mProductMap[eType];
}

image_map_t RendererBase::GetProducts()
{
    auto imageIndex = USwapChain->GetCurrentFrame();
    auto mProductMap = m_vImages.at(imageIndex);
    return mProductMap;
}

void RendererBase::CreateSampler()
{
    UDevice->CreateSampler(m_Sampler, 1,  vk::SamplerAddressMode::eClampToEdge);
}

std::shared_ptr<Image> RendererBase::CreateImage(attachment_t attachment)
{
    auto texture = std::make_shared<Image>();
    ktxTexture *offscreen;
    vk::Format imageFormat;
    ImageLoader::AllocateRawDataAsKTXTexture(&offscreen, &imageFormat, out_extent.width, out_extent.height, 1, 2, VulkanStaticHelper::VkFormatToGLFormat(attachment.format));

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
        framebufferCI.width = out_extent.width;
        framebufferCI.height = out_extent.height;
        framebufferCI.layers = 1;

        m_vFramebuffers[frame] = UDevice->Make<vk::Framebuffer, vk::FramebufferCreateInfo>(framebufferCI);
    }
}