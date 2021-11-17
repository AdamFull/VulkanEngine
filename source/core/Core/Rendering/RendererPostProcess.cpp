#include "RendererPostProcess.h"
#include "Core/VulkanAllocator.h"

using namespace Engine::Core;
using namespace Engine::Core::Renderer;
using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;

RendererPostProcess::RendererPostProcess(std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapchain)
{
    m_device = device;
    m_swapchain = swapchain;
}

void RendererPostProcess::Create()
{
    vAttachments = 
    {
        {
            ETextureAttachmentType::ePosition,
            FAttachmentInfo
            (
                vk::Format::eR16G16B16A16Sfloat,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        },
        {
            ETextureAttachmentType::eNormal,
            FAttachmentInfo
            (
                vk::Format::eR16G16B16A16Sfloat,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        },
        {
            ETextureAttachmentType::eDiffuseAlbedo,
            FAttachmentInfo
            (
                vk::Format::eR8G8B8A8Snorm,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        }
    };

    RendererBase::Create();
}

void RendererPostProcess::ReCreate()
{
    RendererBase::ReCreate();
}

void RendererPostProcess::Update(vk::CommandBuffer& commandBuffer, void* ubo)
{
    RendererBase::Update(commandBuffer, ubo);
}

void RendererPostProcess::Cleanup()
{
    RendererBase::Cleanup();
}


void RendererPostProcess::CreateRenderPass()
{
    RendererBase::CreateRenderPass();

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
		desc.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        descTemporary.emplace(attachment, desc);
	}

    //Depth source
    desc.format = m_pDepthImage->GetParams().format;
    desc.initialLayout = vk::ImageLayout::eUndefined;
	desc.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    descTemporary.emplace(ETextureAttachmentType::eDepth, desc);

    std::vector<vk::AttachmentDescription> vAttachmentDesc{};
    std::transform(descTemporary.begin(), descTemporary.end(), std::back_inserter(vAttachmentDesc), get_map_values(descTemporary));

    std::vector<vk::AttachmentReference> colorReferences{};

    for(uint32_t i = 0; i < vAttachments.size(); i++)
        colorReferences.emplace_back(vk::AttachmentReference{i, vk::ImageLayout::eColorAttachmentOptimal});

    vk::AttachmentReference depthReference = {};
	depthReference.attachment = vAttachments.size();
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

    renderPass = m_device->GetLogical().createRenderPass(renderPassCI);
}

void RendererPostProcess::CreateFrameBuffers()
{
    RendererBase::CreateFrameBuffers();
    auto framesInFlight = m_swapchain->GetFramesInFlight();
    auto extent = m_swapchain->GetExtent();

    m_vFramebuffers.resize(framesInFlight);
    for(size_t frame = 0; frame < framesInFlight; frame++)
    {
        std::vector<vk::ImageView> vImages{};
        auto& buffer = m_vImages.at(frame);

        for(auto& [attachment, texture] : buffer)
            vImages.push_back(texture->GetDescriptor().imageView);

        vImages.push_back(m_pDepthImage->GetDescriptor().imageView);

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

void RendererPostProcess::CreateUniformBuffers()
{
    RendererBase::CreateUniformBuffers();
    auto framesInFlight = m_swapchain->GetFramesInFlight();
    m_pUniform->Create(framesInFlight, sizeof(FLightningData));
}

void RendererPostProcess::CreateMaterial()
{
    RendererBase::CreateMaterial();

    m_pMaterial = FDefaultAllocator::Allocate<MaterialDeferred>();
    m_pMaterial->Create(nullptr);
}