#include "VulkanSwapChain.h"
#include "VulkanDevice.h"
#include "WindowHandle.h"
#include "VulkanStaticHelper.h"

namespace Engine
{
    void SwapChain::Create(std::unique_ptr<Device>& device)
    {
        CreateSwapChain(device);
        CreateSwapChainImageViews(device);
        CreateMSAAResources(device);
        CreateDepthResources(device);
        CreateRenderPass(device);
        CreateFrameBuffers(device);
        CreateSyncObjects(device);
    }

    vk::Result SwapChain::AcquireNextImage(std::unique_ptr<Device>& device, uint32_t *imageIndex)
    {
        device->GetLogical()->waitForFences(1, &data.vInFlightFences[data.currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

        auto result = device->GetLogical()->acquireNextImageKHR(
            data.swapChain, std::numeric_limits<uint64_t>::max(),
            data.vImageAvailableSemaphores[data.currentFrame], nullptr, imageIndex);

        return result;
    }

    vk::Result SwapChain::SubmitCommandBuffers(std::unique_ptr<Device>& device, const vk::CommandBuffer* commandBuffer, uint32_t *imageIndex)
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

        //TODO: Handle this result
        device->GetLogical()->resetFences(1, &data.vInFlightFences[data.currentFrame]);

        try
        {
            device->GetGraphicsQueue().submit(submitInfo, data.vInFlightFences[data.currentFrame]);
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
        resultPresent = device->GetPresentQueue().presentKHR(presentInfo);

        data.currentFrame = (data.currentFrame + 1) % data.iFramesInFlight;

        return resultPresent;
    }

    void SwapChain::Cleanup(std::unique_ptr<Device>& device)
    {
        //Destroying depth image
        device->Destroy(data.depthImageView);
        device->Destroy(data.depthImage);
        device->Destroy(data.depthImageMemory);

        //Destroying msaa image
        device->Destroy(data.MSAAImageView);
        device->Destroy(data.MSAAImage);
        device->Destroy(data.MSAAImageMemory);

        for (auto framebuffer : data.vFramebuffers)
        {
            device->Destroy(framebuffer);
        }

        device->Destroy(data.renderPass);

        for (auto imageView : data.vImageViews)
        {
            device->Destroy(imageView);
        }

        device->Destroy(data.swapChain);
    }

    void SwapChain::Destroy(std::unique_ptr<Device>& device)
    {
        for (size_t i = 0; i < data.iFramesInFlight; i++)
        {
            device->Destroy(data.vRenderFinishedSemaphores[i]);
            device->Destroy(data.vImageAvailableSemaphores[i]);
            device->Destroy(data.vInFlightFences[i]);
        }
    }

    void SwapChain::ReCreate(std::unique_ptr<Device>& device)
    {
        CreateSwapChain(device);
        CreateSwapChainImageViews(device);
        CreateRenderPass(device);
        CreateMSAAResources(device);
        CreateDepthResources(device);
    }

    void SwapChain::CreateSwapChain(std::unique_ptr<Device>& device)
    {
        assert(device && "Cannot create swap chain, cause logical device is not valid.");
        SwapChainSupportDetails swapChainSupport = device->QuerySwapChainSupport();

        vk::SurfaceFormatKHR surfaceFormat = VulkanStaticHelper::ChooseSwapSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR presentMode = VulkanStaticHelper::ChooseSwapPresentMode(swapChainSupport.presentModes);
        data.extent = ChooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.maxImageCount;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo(
            vk::SwapchainCreateFlagsKHR(),
            device->GetSurface(),
            imageCount,
            surfaceFormat.format,
            surfaceFormat.colorSpace,
            data.extent,
            1, // imageArrayLayers
            vk::ImageUsageFlagBits::eColorAttachment);

        QueueFamilyIndices indices = device->FindQueueFamilies();
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

        data.swapChain = device->Make<vk::SwapchainKHR, vk::SwapchainCreateInfoKHR>(createInfo);
        assert(data.swapChain && "Swap chain was not created");

        data.vImages = device->Make<std::vector<vk::Image>, vk::SwapchainKHR>(data.swapChain);
        assert(!data.vImages.empty() && "Swap chain images was not created");

        data.imageFormat = surfaceFormat.format;
    }

    void SwapChain::CreateSwapChainImageViews(std::unique_ptr<Device>& device)
    {
        data.vImageViews.resize(data.vImages.size());

        for (size_t i = 0; i < data.vImages.size(); i++)
        {
            data.vImageViews[i] = device->CreateImageView(data.vImages[i], 1, data.imageFormat, vk::ImageAspectFlagBits::eColor);
        }
    }

    void SwapChain::CreateMSAAResources(std::unique_ptr<Device>& device)
    {
        vk::Format colorFormat = data.imageFormat;

        device->CreateImage(data.MSAAImage, data.MSAAImageMemory, vk::Extent3D{data.extent, 0}, 1, 
        device->GetSamples(), colorFormat, vk::ImageTiling::eOptimal, 
        vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment, 
        vk::MemoryPropertyFlagBits::eDeviceLocal);

        data.MSAAImageView = device->CreateImageView(data.MSAAImage, 1, colorFormat, vk::ImageAspectFlagBits::eColor);
    }

    void SwapChain::CreateDepthResources(std::unique_ptr<Device>& device)
    {
        vk::Format depthFormat = device->FindSupportedFormat(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);

        device->CreateImage(data.depthImage, data.depthImageMemory, vk::Extent3D{data.extent, 0}, 1,
        device->GetSamples(), depthFormat, vk::ImageTiling::eOptimal, 
        vk::ImageUsageFlagBits::eDepthStencilAttachment,  vk::MemoryPropertyFlagBits::eDeviceLocal);

        data.depthImageView = device->CreateImageView(data.depthImage, 1, depthFormat, vk::ImageAspectFlagBits::eDepth);

        device->TransitionImageLayout(data.depthImage, 1, vk::ImageAspectFlagBits::eDepth, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }

    void SwapChain::CreateRenderPass(std::unique_ptr<Device>& device)
    {
        assert(device && "Cannot create render pass, cause logical device is not valid.");
        vk::AttachmentDescription colorAttachment = {};
        colorAttachment.format = data.imageFormat;
        colorAttachment.samples = device->GetSamples();
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentDescription depthAttachment{};
        depthAttachment.format = device->FindSupportedFormat(
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
        depthAttachment.samples = device->GetSamples();
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = data.imageFormat;
        colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
        colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpass = {};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;

        vk::SubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite; //vk::AccessFlagBits::eColorAttachmentRead

        std::array<vk::AttachmentDescription, 3> attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};

        vk::RenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        ;
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        data.renderPass = device->Make<vk::RenderPass, vk::RenderPassCreateInfo>(renderPassInfo);
        assert(data.renderPass && "Render pass was not created");
    }

    void SwapChain::CreateFrameBuffers(std::unique_ptr<Device>& device)
    {
        assert(!data.vImageViews.empty() && "Cannot create framebuffers, cause swap chain image views are empty.");
        assert(device && "Cannot create framebuffers, cause logical device is not valid.");
        assert(data.renderPass && "Cannot create framebuffers, cause render pass is not valid.");
        assert(data.MSAAImageView && "Cannot create framebuffers, cause msaa image view is not valid.");
        assert(data.depthImageView && "Cannot create framebuffers, cause depth image view is not valid.");

        data.vFramebuffers.resize(data.vImageViews.size());

        for (size_t i = 0; i < data.vImageViews.size(); i++)
        {
            std::array<vk::ImageView, 3> attachments =
            {
                data.MSAAImageView,
                data.depthImageView,
                data.vImageViews[i]
            };

            vk::FramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.renderPass = data.renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = data.extent.width;
            framebufferInfo.height = data.extent.height;
            framebufferInfo.layers = 1;

            data.vFramebuffers[i] = device->Make<vk::Framebuffer, vk::FramebufferCreateInfo>(framebufferInfo);
            assert(data.vFramebuffers[i] && "Failed while creating framebuffer");
        }
    }

    void SwapChain::CreateSyncObjects(std::unique_ptr<Device>& device)
    {
        data.vImageAvailableSemaphores.resize(data.iFramesInFlight);
        data.vRenderFinishedSemaphores.resize(data.iFramesInFlight);
        data.vInFlightFences.resize(data.iFramesInFlight);

        try
        {
            for (size_t i = 0; i < data.iFramesInFlight; i++)
            {
                data.vImageAvailableSemaphores[i] = device->Make<vk::Semaphore, vk::SemaphoreCreateInfo>(vk::SemaphoreCreateInfo{});
                data.vRenderFinishedSemaphores[i] = device->Make<vk::Semaphore, vk::SemaphoreCreateInfo>(vk::SemaphoreCreateInfo{});
                data.vInFlightFences[i] = device->Make<vk::Fence, vk::FenceCreateInfo>(vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled});
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
}