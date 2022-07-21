#include "VulkanStaticHelper.h"
#include "VulkanHighLevel.h"
#include "Image/Image.h"

using namespace Engine::Core;
using namespace Engine::Core::Window;

template<>
std::unique_ptr<CSwapChain> utl::singleton<CSwapChain>::_instance{nullptr};

CSwapChain::~CSwapChain()
{
    cleanup();

    for (size_t i = 0; i < m_iFramesInFlight; i++)
    {
        CDevice::getInstance()->destroy(m_vRenderFinishedSemaphores[i]);
        CDevice::getInstance()->destroy(m_vImageAvailableSemaphores[i]);
        CDevice::getInstance()->destroy(m_vInFlightFences[i]);
    }
}

void CSwapChain::create()
{
    createSwapChain();
    createSwapChainImageViews();
    createSyncObjects();
}

vk::Result CSwapChain::acquireNextImage(uint32_t *imageIndex)
{
    auto res = CDevice::getInstance()->getLogical().waitForFences(1, &m_vInFlightFences[m_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    auto result = CDevice::getInstance()->getLogical().acquireNextImageKHR(
        m_swapChain, std::numeric_limits<uint64_t>::max(),
        m_vImageAvailableSemaphores[m_currentFrame], nullptr, imageIndex);

    return result;
}

vk::Result CSwapChain::submitCommandBuffers(const vk::CommandBuffer *commandBuffer, uint32_t *imageIndex, vk::QueueFlagBits queueBit)
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
    auto result = CDevice::getInstance()->getLogical().resetFences(1, &m_vInFlightFences[m_currentFrame]);

    try
    {
        vk::Queue queue{};
        switch (queueBit)
        {
        case vk::QueueFlagBits::eGraphics: {
            queue = CDevice::getInstance()->getGraphicsQueue();
        } break;
        case vk::QueueFlagBits::eCompute: {
            queue = CDevice::getInstance()->getComputeQueue();
        } break;
        case vk::QueueFlagBits::eTransfer: {
            queue = CDevice::getInstance()->getTransferQueue();
        } break;
        }
        queue.submit(submitInfo, m_vInFlightFences[m_currentFrame]);
        //CDevice::getInstance()->GetGraphicsQueue().submit(submitInfo, m_vInFlightFences[m_currentFrame]);
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
    resultPresent = CDevice::getInstance()->getPresentQueue().presentKHR(presentInfo);

    m_currentFrame = (m_currentFrame + 1) % m_iFramesInFlight;

    return resultPresent;
}

void CSwapChain::cleanup()
{
    for (auto imageView : m_vImageViews)
        CDevice::getInstance()->destroy(imageView);

    CDevice::getInstance()->destroy(m_swapChain);
}

void CSwapChain::reCreate()
{
    createSwapChain();
    createSwapChainImageViews();
}

void CSwapChain::createSwapChain()
{
    assert(CDevice::getInstance() && "Cannot create swap chain, cause logical device is not valid.");
    SwapChainSupportDetails swapChainSupport = CDevice::getInstance()->querySwapChainSupport();

    vk::SurfaceFormatKHR surfaceFormat = VulkanStaticHelper::ChooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = VulkanStaticHelper::ChooseSwapPresentMode(swapChainSupport.presentModes);
    m_extent = chooseSwapExtent(swapChainSupport.capabilities);
    //m_extent = vk::Extent2D{m_extent.width / 2, m_extent.height / 2};

    uint32_t imageCount = m_iFramesInFlight; // swapChainSupport.capabilities.maxImageCount;
    /*if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }*/

    vk::SwapchainCreateInfoKHR createInfo(
        vk::SwapchainCreateFlagsKHR(),
        CDevice::getInstance()->getSurface(),
        imageCount,
        surfaceFormat.format,
        surfaceFormat.colorSpace,
        m_extent,
        1, // imageArrayLayers
        vk::ImageUsageFlagBits::eColorAttachment);

    QueueFamilyIndices indices = CDevice::getInstance()->findQueueFamilies();
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

    m_swapChain = CDevice::getInstance()->make<vk::SwapchainKHR, vk::SwapchainCreateInfoKHR>(createInfo);
    assert(m_swapChain && "Swap chain was not created");

    m_vImages = CDevice::getInstance()->make<std::vector<vk::Image>, vk::SwapchainKHR>(m_swapChain);
    assert(!m_vImages.empty() && "Swap chain images was not created");

    m_imageFormat = surfaceFormat.format;
}

void CSwapChain::createSwapChainImageViews()
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

        m_vImageViews[i] = CImage::createImageView(m_vImages[i], viewInfo);
    }
}

void CSwapChain::createSyncObjects()
{
    m_vImageAvailableSemaphores.resize(m_iFramesInFlight);
    m_vRenderFinishedSemaphores.resize(m_iFramesInFlight);
    m_vInFlightFences.resize(m_iFramesInFlight);

    try
    {
        for (size_t i = 0; i < m_iFramesInFlight; i++)
        {
            m_vImageAvailableSemaphores[i] = CDevice::getInstance()->make<vk::Semaphore, vk::SemaphoreCreateInfo>(vk::SemaphoreCreateInfo{});
            m_vRenderFinishedSemaphores[i] = CDevice::getInstance()->make<vk::Semaphore, vk::SemaphoreCreateInfo>(vk::SemaphoreCreateInfo{});
            m_vInFlightFences[i] = CDevice::getInstance()->make<vk::Fence, vk::FenceCreateInfo>(vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled});
        }
    }
    catch (vk::SystemError err)
    {
        throw std::runtime_error("Failed to create synchronization objects for a frame!");
    }
}

vk::Extent2D CSwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        vk::Extent2D actualExtent = {static_cast<uint32_t>(CWindowHandle::m_iWidth), static_cast<uint32_t>(CWindowHandle::m_iHeight)};

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}