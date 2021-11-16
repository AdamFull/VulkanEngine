#include "VulkanSwapChain.h"
#include "VulkanStaticHelper.h"
#include "VulkanAllocator.h"

using namespace Engine::Core;
using namespace Engine::Core::Window;

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
    CreateRenderPass();
    CreateFrameBuffers();
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

    m_device->Destroy(data.renderPass);

    for (auto imageView : data.vImageViews)
        m_device->Destroy(imageView);

    m_device->Destroy(data.swapChain);
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