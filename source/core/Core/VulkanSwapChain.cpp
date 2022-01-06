#include "VulkanStaticHelper.h"
#include "VulkanHighLevel.h"
#include "Image/Image.h"

using namespace Engine::Core;
using namespace Engine::Core::Window;

SwapChain::~SwapChain()
{
    Cleanup();

    for (size_t i = 0; i < m_iFramesInFlight; i++)
    {
        UDevice->Destroy(m_vRenderFinishedSemaphores[i]);
        UDevice->Destroy(m_vImageAvailableSemaphores[i]);
        UDevice->Destroy(m_vInFlightFences[i]);
    }
}

void SwapChain::Create()
{
    CreateSwapChain();
    CreateSwapChainImageViews();
    CreateDepthResources();
    CreateRenderPass();
    CreateFrameBuffers();
    CreateSyncObjects();
}

vk::Result SwapChain::AcquireNextImage(uint32_t *imageIndex)
{
    auto res = UDevice->GetLogical().waitForFences(1, &m_vInFlightFences[m_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    auto result = UDevice->GetLogical().acquireNextImageKHR(
        m_swapChain, std::numeric_limits<uint64_t>::max(),
        m_vImageAvailableSemaphores[m_currentFrame], nullptr, imageIndex);

    return result;
}

vk::Result SwapChain::SubmitCommandBuffers(const vk::CommandBuffer *commandBuffer, uint32_t *imageIndex)
{
    vk::SubmitInfo submitInfo = {};

    vk::Semaphore waitSemaphores[] = {m_vImageAvailableSemaphores[m_currentFrame]};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffer;

    vk::Semaphore signalSemaphores[] = {m_vRenderFinishedSemaphores[m_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // TODO: Handle this result
    auto result = UDevice->GetLogical().resetFences(1, &m_vInFlightFences[m_currentFrame]);

    try
    {
        UDevice->GetGraphicsQueue().submit(submitInfo, m_vInFlightFences[m_currentFrame]);
    }
    catch (vk::SystemError err)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    vk::PresentInfoKHR presentInfo = {};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    vk::SwapchainKHR swapChains[] = {m_swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = imageIndex;

    vk::Result resultPresent;
    resultPresent = UDevice->GetPresentQueue().presentKHR(presentInfo);

    m_currentFrame = (m_currentFrame + 1) % m_iFramesInFlight;

    return resultPresent;
}

void SwapChain::Cleanup()
{
    // Destroying depth image
    UDevice->Destroy(m_depthImageView);
    UDevice->Destroy(m_depthImage);
    UDevice->Destroy(m_depthImageMemory);

    for (auto framebuffer : m_vFramebuffers)
        UDevice->Destroy(framebuffer);

    UDevice->Destroy(m_renderPass);

    for (auto imageView : m_vImageViews)
        UDevice->Destroy(imageView);

    UDevice->Destroy(m_swapChain);
}

void SwapChain::ReCreate()
{
    CreateSwapChain();
    CreateSwapChainImageViews();
    CreateRenderPass();
    CreateDepthResources();
    CreateFrameBuffers();
}

void SwapChain::CreateSwapChain()
{
    assert(UDevice && "Cannot create swap chain, cause logical device is not valid.");
    SwapChainSupportDetails swapChainSupport = UDevice->QuerySwapChainSupport();

    vk::SurfaceFormatKHR surfaceFormat = VulkanStaticHelper::ChooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = VulkanStaticHelper::ChooseSwapPresentMode(swapChainSupport.presentModes);
    m_extent = ChooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = m_iFramesInFlight; // swapChainSupport.capabilities.maxImageCount;
    /*if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }*/

    vk::SwapchainCreateInfoKHR createInfo(
        vk::SwapchainCreateFlagsKHR(),
        UDevice->GetSurface(),
        imageCount,
        surfaceFormat.format,
        surfaceFormat.colorSpace,
        m_extent,
        1, // imageArrayLayers
        vk::ImageUsageFlagBits::eColorAttachment);

    QueueFamilyIndices indices = UDevice->FindQueueFamilies();
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

    m_swapChain = UDevice->Make<vk::SwapchainKHR, vk::SwapchainCreateInfoKHR>(createInfo);
    assert(m_swapChain && "Swap chain was not created");

    m_vImages = UDevice->Make<std::vector<vk::Image>, vk::SwapchainKHR>(m_swapChain);
    assert(!m_vImages.empty() && "Swap chain images was not created");

    m_imageFormat = surfaceFormat.format;
}

void SwapChain::CreateSwapChainImageViews()
{
    m_vImageViews.resize(m_vImages.size());

    for (size_t i = 0; i < m_vImages.size(); i++)
    {
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = m_imageFormat;
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        m_vImageViews[i] = Image::CreateImageView(m_vImages[i], viewInfo);
    }
}

void SwapChain::CreateDepthResources()
{
    vk::Format depthFormat = Image::GetDepthFormat();

    vk::ImageCreateInfo imageInfo{};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = m_extent.width;
    imageInfo.extent.height = m_extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = depthFormat;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.samples = vk::SampleCountFlagBits::e1;

    Image::CreateImage(m_depthImage, m_depthImageMemory, imageInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = depthFormat;
    viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    m_depthImageView = Image::CreateImageView(m_depthImage, viewInfo);

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

    Image::TransitionImageLayout(m_depthImage, vBarriers, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
}

void SwapChain::CreateRenderPass()
{
    assert(UDevice && "Cannot create render pass, cause logical device is not valid.");
    vk::AttachmentDescription colorAttachment = {};
    colorAttachment.format = m_imageFormat;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR; //vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentDescription depthAttachment{};
    depthAttachment.format = Image::GetDepthFormat();
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

    m_renderPass = UDevice->Make<vk::RenderPass, vk::RenderPassCreateInfo>(renderPassInfo);
    assert(m_renderPass && "Render pass was not created");
}

void SwapChain::CreateFrameBuffers()
{
    assert(!m_vImageViews.empty() && "Cannot create framebuffers, cause swap chain image views are empty.");
    assert(UDevice && "Cannot create framebuffers, cause logical device is not valid.");
    assert(m_renderPass && "Cannot create framebuffers, cause render pass is not valid.");
    assert(m_depthImageView && "Cannot create framebuffers, cause depth image view is not valid.");

    m_vFramebuffers.resize(m_vImageViews.size());

    for (size_t i = 0; i < m_vImageViews.size(); i++)
    {
        std::array<vk::ImageView, 2> attachments =
            {
                m_vImageViews[i],
                m_depthImageView
            };

        vk::FramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_extent.width;
        framebufferInfo.height = m_extent.height;
        framebufferInfo.layers = 1;

        m_vFramebuffers[i] = UDevice->Make<vk::Framebuffer, vk::FramebufferCreateInfo>(framebufferInfo);
        assert(m_vFramebuffers[i] && "Failed while creating framebuffer");
    }
}

void SwapChain::CreateSyncObjects()
{
    m_vImageAvailableSemaphores.resize(m_iFramesInFlight);
    m_vRenderFinishedSemaphores.resize(m_iFramesInFlight);
    m_vInFlightFences.resize(m_iFramesInFlight);

    try
    {
        for (size_t i = 0; i < m_iFramesInFlight; i++)
        {
            m_vImageAvailableSemaphores[i] = UDevice->Make<vk::Semaphore, vk::SemaphoreCreateInfo>(vk::SemaphoreCreateInfo{});
            m_vRenderFinishedSemaphores[i] = UDevice->Make<vk::Semaphore, vk::SemaphoreCreateInfo>(vk::SemaphoreCreateInfo{});
            m_vInFlightFences[i] = UDevice->Make<vk::Fence, vk::FenceCreateInfo>(vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled});
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