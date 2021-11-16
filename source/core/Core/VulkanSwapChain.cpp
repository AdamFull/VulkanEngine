#include "VulkanSwapChain.h"
#include "Resources/Textures/ImageLoader.h"
#include "VulkanStaticHelper.h"
#include "VulkanAllocator.h"

using namespace Engine::Core;
using namespace Engine::Core::Window;
using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;
using namespace Engine::Resources::Loaders;

//Helpers from https://stackoverflow.com/questions/771453/copy-map-values-to-vector-in-stl
template<typename tPair>
struct second_t 
{
    typename tPair::second_type operator()(const tPair& p) const 
    { 
        return p.second; 
    }
};

template<typename tMap> 
second_t<typename tMap::value_type> get_map_values(const tMap& m ) 
{ 
    return second_t<typename tMap::value_type>(); 
}

std::map<ETextureAttachmentType, FAttachmentInfo> SwapChain::vAttachments =
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
        ETextureAttachmentType::eLightningMask,
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

SwapChain::SwapChain(std::shared_ptr<Device> device) : m_device(device)
{

}

SwapChain::~SwapChain()
{
    Cleanup();

    for (size_t i = 0; i < data.iFramesInFlight; i++)
    {
        m_device->Destroy(data.vRenderFinishedSemaphores[i]);
        m_device->Destroy(data.vImageAvailableSemaphores[i]);
        m_device->Destroy(data.vInFlightFences[i]);
    }
}

void SwapChain::Create()
{
    CreateSwapChain();
    CreateSwapChainImageViews();
    CreateDepthResources();
    CreateCompositionSampler();
    CreateOffscreenImages();
    CreateRenderPass();
    CreateOffscreenRenderPass();
    CreateCompositionMaterial();
    CreateFrameBuffers();
    CreateOffscreenFrameBuffers();
    CreateSyncObjects();
}

vk::Result SwapChain::AcquireNextImage(uint32_t *imageIndex)
{
    m_device->GetLogical().waitForFences(1, &data.vInFlightFences[data.currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    auto result = m_device->GetLogical().acquireNextImageKHR(
        data.swapChain, std::numeric_limits<uint64_t>::max(),
        data.vImageAvailableSemaphores[data.currentFrame], nullptr, imageIndex);

    return result;
}

vk::Result SwapChain::SubmitCommandBuffers(const vk::CommandBuffer *commandBuffer, uint32_t *imageIndex)
{
    vk::SubmitInfo submitInfo = {};

    vk::Semaphore waitSemaphores[] = {data.vImageAvailableSemaphores[data.currentFrame]};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffer;

    vk::Semaphore signalSemaphores[] = {data.vRenderFinishedSemaphores[data.currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // TODO: Handle this result
    m_device->GetLogical().resetFences(1, &data.vInFlightFences[data.currentFrame]);

    try
    {
        m_device->GetGraphicsQueue().submit(submitInfo, data.vInFlightFences[data.currentFrame]);
    }
    catch (vk::SystemError err)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    vk::PresentInfoKHR presentInfo = {};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    vk::SwapchainKHR swapChains[] = {data.swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = imageIndex;

    vk::Result resultPresent;
    resultPresent = m_device->GetPresentQueue().presentKHR(presentInfo);

    data.currentFrame = (data.currentFrame + 1) % data.iFramesInFlight;

    return resultPresent;
}

void SwapChain::Cleanup()
{
    // Destroying depth image
    m_device->Destroy(data.depthImageView);
    m_device->Destroy(data.depthImage);
    m_device->Destroy(data.depthImageMemory);

    for (auto framebuffer : data.vFramebuffers)
        m_device->Destroy(framebuffer);

    for (auto framebuffer : data.vOffscreenFramebuffers)
        m_device->Destroy(framebuffer);

    m_device->Destroy(data.renderPass);
    m_device->Destroy(data.offscreenRenderPass);

    for (auto imageView : data.vImageViews)
        m_device->Destroy(imageView);
    
    //Cleaning g-buffer
    m_vGBuffer.clear();
    m_pGBufferDepth->Cleanup();
    m_device->Destroy(compositionSampler);

    m_device->Destroy(data.swapChain);
}

void SwapChain::ReCreate()
{
    CreateSwapChain();
    CreateSwapChainImageViews();
    CreateCompositionSampler();
    CreateOffscreenImages();
    CreateRenderPass();
    CreateOffscreenRenderPass();
    CreateDepthResources();
    CreateFrameBuffers();
    CreateOffscreenFrameBuffers();
    m_pComposition->ReCreate();
}

void SwapChain::CreateSwapChain()
{
    assert(m_device && "Cannot create swap chain, cause logical device is not valid.");
    SwapChainSupportDetails swapChainSupport = m_device->QuerySwapChainSupport();

    vk::SurfaceFormatKHR surfaceFormat = VulkanStaticHelper::ChooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = VulkanStaticHelper::ChooseSwapPresentMode(swapChainSupport.presentModes);
    data.extent = ChooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = data.iFramesInFlight; // swapChainSupport.capabilities.maxImageCount;
    /*if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }*/

    vk::SwapchainCreateInfoKHR createInfo(
        vk::SwapchainCreateFlagsKHR(),
        m_device->GetSurface(),
        imageCount,
        surfaceFormat.format,
        surfaceFormat.colorSpace,
        data.extent,
        1, // imageArrayLayers
        vk::ImageUsageFlagBits::eColorAttachment);

    QueueFamilyIndices indices = m_device->FindQueueFamilies();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

    data.swapChain = m_device->Make<vk::SwapchainKHR, vk::SwapchainCreateInfoKHR>(createInfo);
    assert(data.swapChain && "Swap chain was not created");

    data.vImages = m_device->Make<std::vector<vk::Image>, vk::SwapchainKHR>(data.swapChain);
    assert(!data.vImages.empty() && "Swap chain images was not created");

    data.imageFormat = surfaceFormat.format;
}

void SwapChain::CreateSwapChainImageViews()
{
    data.vImageViews.resize(data.vImages.size());

    for (size_t i = 0; i < data.vImages.size(); i++)
    {
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = data.imageFormat;
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        data.vImageViews[i] = m_device->CreateImageView(data.vImages[i], viewInfo);
    }
}

void SwapChain::CreateDepthResources()
{
    vk::Format depthFormat = m_device->GetDepthFormat();

    vk::ImageCreateInfo imageInfo{};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = data.extent.width;
    imageInfo.extent.height = data.extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = depthFormat;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.samples = vk::SampleCountFlagBits::e1;

    m_device->CreateImage(data.depthImage, data.depthImageMemory, imageInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = depthFormat;
    viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    data.depthImageView = m_device->CreateImageView(data.depthImage, viewInfo);

    std::vector<vk::ImageMemoryBarrier> vBarriers;
    vk::ImageMemoryBarrier barrier{};
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    vBarriers.push_back(barrier);

    m_device->TransitionImageLayout(data.depthImage, vBarriers, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
}

void SwapChain::CreateRenderPass()
{
    assert(m_device && "Cannot create render pass, cause logical device is not valid.");
    vk::AttachmentDescription colorAttachment = {};
    colorAttachment.format = data.imageFormat;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR; //vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentDescription depthAttachment{};
    depthAttachment.format = m_device->GetDepthFormat();
    depthAttachment.samples =vk::SampleCountFlagBits::e1;
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::SubpassDescription subpass = {};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    vk::SubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite; // vk::AccessFlagBits::eColorAttachmentRead

    std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    vk::RenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    ;
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    data.renderPass = m_device->Make<vk::RenderPass, vk::RenderPassCreateInfo>(renderPassInfo);
    assert(data.renderPass && "Render pass was not created");
}

void SwapChain::CreateOffscreenRenderPass()
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
		desc.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        descTemporary.emplace(attachment, desc);
	}

    //Depth source
    desc.format = m_pGBufferDepth->GetParams().format;
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

    data.offscreenRenderPass = m_device->GetLogical().createRenderPass(renderPassCI);
}

void SwapChain::CreateFrameBuffers()
{
    assert(!data.vImageViews.empty() && "Cannot create framebuffers, cause swap chain image views are empty.");
    assert(m_device && "Cannot create framebuffers, cause logical device is not valid.");
    assert(data.renderPass && "Cannot create framebuffers, cause render pass is not valid.");
    assert(data.depthImageView && "Cannot create framebuffers, cause depth image view is not valid.");

    data.vFramebuffers.resize(data.vImageViews.size());

    for (size_t i = 0; i < data.vImageViews.size(); i++)
    {
        std::array<vk::ImageView, 2> attachments =
            {
                data.vImageViews[i],
                data.depthImageView
            };

        vk::FramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.renderPass = data.renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = data.extent.width;
        framebufferInfo.height = data.extent.height;
        framebufferInfo.layers = 1;

        data.vFramebuffers[i] = m_device->Make<vk::Framebuffer, vk::FramebufferCreateInfo>(framebufferInfo);
        assert(data.vFramebuffers[i] && "Failed while creating framebuffer");
    }
}

void SwapChain::CreateOffscreenFrameBuffers()
{
    assert(!data.vImageViews.empty() && "Cannot create framebuffers, cause swap chain image views are empty.");
    assert(m_device && "Cannot create framebuffers, cause logical device is not valid.");
    assert(data.offscreenRenderPass && "Cannot create framebuffers, cause render pass is not valid.");
    assert(!m_vGBuffer.empty() && "Cannot create framebuffers, cause g-buffer images is not created.");

    data.vOffscreenFramebuffers.resize(data.iFramesInFlight);
    for(size_t frame = 0; frame < data.iFramesInFlight; frame++)
    {
        std::vector<vk::ImageView> vImages{};
        auto& gbuffer = m_vGBuffer.at(frame);

        for(auto& [attachment, texture] : gbuffer)
            vImages.push_back(texture->GetDescriptor().imageView);

        vImages.push_back(m_pGBufferDepth->GetDescriptor().imageView);

        vk::FramebufferCreateInfo framebufferCI = {};
        framebufferCI.pNext = nullptr;
        framebufferCI.renderPass = data.offscreenRenderPass;
        framebufferCI.pAttachments = vImages.data();
        framebufferCI.attachmentCount = static_cast<uint32_t>(vImages.size());
        framebufferCI.width = data.extent.width;
        framebufferCI.height = data.extent.height;
        framebufferCI.layers = 1;

        data.vOffscreenFramebuffers[frame] = m_device->Make<vk::Framebuffer, vk::FramebufferCreateInfo>(framebufferCI);
    }
}

void SwapChain::CreateCompositionSampler()
{
    m_device->CreateSampler(compositionSampler, 1,  vk::SamplerAddressMode::eClampToEdge);
}

std::shared_ptr<TextureBase> SwapChain::CreateOffscreenImage(vk::Format format, vk::ImageUsageFlags usage)
{
    auto texture = FDefaultAllocator::Allocate<TextureBase>();
    ktxTexture *offscreen;
    vk::Format imageFormat;
    ImageLoader::AllocateRawDataAsKTXTexture(&offscreen, &imageFormat, data.extent.width, data.extent.height, 1, 2, VulkanStaticHelper::VkFormatToGLFormat(format));

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

    texture->SetSampler(compositionSampler);
    texture->InitializeTexture(offscreen, imageFormat, usage, aspectMask);
    if(usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
        texture->TransitionImageLayout(imageLayout, aspectMask, false);
    else
        texture->SetImageLayout(imageLayout);

    texture->UpdateDescriptor();
    return texture;
}

void SwapChain::CreateOffscreenImages()
{
    m_vGBuffer.resize(data.iFramesInFlight);
    for(uint32_t frame = 0; frame < data.iFramesInFlight; frame++)
    {
        gbuffer_t mGBuffer;
        for(auto& [attachment, param] : vAttachments)
            mGBuffer.emplace(attachment, CreateOffscreenImage(param.format, param.usage));
        m_vGBuffer[frame] = std::move(mGBuffer);
    }

    auto depthFormat = m_device->GetDepthFormat();
    m_pGBufferDepth = CreateOffscreenImage(depthFormat, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled);
}

void SwapChain::CreateCompositionMaterial()
{
    assert(!m_vGBuffer.empty() && "Composition material cannot be created, cause g-buffer images is not created.");

    m_pComposition = FDefaultAllocator::Allocate<MaterialDeferred>();
    m_pComposition->Create(nullptr);
}

void SwapChain::UpdateCompositionMaterial(vk::CommandBuffer& commandBuffer)
{
    for(auto& [attachment, texture] : m_vGBuffer.at(data.currentFrame))
        m_pComposition->AddTexture(attachment, texture);
    
    //TODO: move uniform buffer to another place
    auto& buffer = ULightUniform->GetUniformBuffer(data.currentFrame);
    m_pComposition->Update(buffer->GetDscriptor(), data.currentFrame);
    m_pComposition->Bind(commandBuffer, data.currentFrame);
}

void SwapChain::CreateSyncObjects()
{
    data.vImageAvailableSemaphores.resize(data.iFramesInFlight);
    data.vRenderFinishedSemaphores.resize(data.iFramesInFlight);
    data.vInFlightFences.resize(data.iFramesInFlight);

    try
    {
        for (size_t i = 0; i < data.iFramesInFlight; i++)
        {
            data.vImageAvailableSemaphores[i] = m_device->Make<vk::Semaphore, vk::SemaphoreCreateInfo>(vk::SemaphoreCreateInfo{});
            data.vRenderFinishedSemaphores[i] = m_device->Make<vk::Semaphore, vk::SemaphoreCreateInfo>(vk::SemaphoreCreateInfo{});
            data.vInFlightFences[i] = m_device->Make<vk::Fence, vk::FenceCreateInfo>(vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled});
        }
    }
    catch (vk::SystemError err)
    {
        throw std::runtime_error("Failed to create synchronization objects for a frame!");
    }
}

vk::Extent2D SwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        vk::Extent2D actualExtent = {static_cast<uint32_t>(WindowHandle::m_iWidth), static_cast<uint32_t>(WindowHandle::m_iHeight)};

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}