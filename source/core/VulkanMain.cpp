#include "VulkanMain.h"
#include "WindowHandle.h"
#include "VulkanDevice.h"
#include "DataTypes/VulkanVertex.h"

namespace Engine::Main
{
    void CreateWindowHandle(FVulkanEngine& engine, int width, int height, const char* srWinName)
    {
        Window::Initialize(engine, width, height, srWinName);
    }

    //Device
    void CreateInstance(FVulkanEngine& engine)
    {
        if (Device::bEnableValidationLayers && !Device::CheckValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }

        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName = "Vulkan";
        appInfo.applicationVersion VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        auto extensions = Device::GetRequiredExtensions();

        vk::InstanceCreateInfo createInfo{};
        createInfo.flags = vk::InstanceCreateFlags();
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (Device::bEnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(Device::vValidationLayers.size());
            createInfo.ppEnabledLayerNames = Device::vValidationLayers.data();
        }

        try
        {
            engine.device.vkInstance = vk::createInstanceUnique(createInfo, nullptr);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create instance!");
        }
    }
    
    void SetupDebugMessenger(FVulkanEngine& engine)
    {
        if (!Device::bEnableValidationLayers)
            return;

        /*auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
            vk::DebugUtilsMessengerCreateFlagsEXT(),
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            ValidationCallback,
            nullptr);

        // NOTE: Vulkan-hpp has methods for this, but they trigger linking errors...
        if (CreateDebugUtilsMessengerEXT(*m_pVkInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, &m_pVkDebugUtils) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to set up debug callback!");
        }*/
    }
    
    void CreateSurface(FVulkanEngine& engine)
    {
        Window::CreateWindowSurface(engine);
    }
    
    void CreateLogicalDevice(FVulkanEngine& engine)
    {
        engine.device.physical = Device::GetPhysicalDevice();
        //engine.device.samples = Device::GetMaxUsableSampleCount();
        QueueFamilyIndices indices = Device::FindQueueFamilies(engine);

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;

        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            queueCreateInfos.push_back({vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority});
        }

        auto deviceFeatures = vk::PhysicalDeviceFeatures();
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.sampleRateShading = VK_TRUE;

        auto createInfo = vk::DeviceCreateInfo(
            vk::DeviceCreateFlags(),
            static_cast<uint32_t>(queueCreateInfos.size()),
            queueCreateInfos.data());

        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(Device::vDeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = Device::vDeviceExtensions.data();

        if (Device::bEnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(Device::vValidationLayers.size());
            createInfo.ppEnabledLayerNames = Device::vValidationLayers.data();
        }

        try
        {
            engine.device.logical = engine.device.physical.createDeviceUnique(createInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create logical device!");
        }

        engine.device.graphicsQueue = engine.device.logical->getQueue(indices.graphicsFamily.value(), 0);
        engine.device.presentQueue = engine.device.logical->getQueue(indices.presentFamily.value(), 0);
    }
    
    void CreateCommandPool(FVulkanEngine& engine)
    {
        QueueFamilyIndices queueFamilyIndices = Device::FindQueueFamilies(engine);

        vk::CommandPoolCreateInfo poolInfo = {};
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        try
        {
            engine.device.commandPool = engine.device.logical->createCommandPool(poolInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create command pool!");
        }
    }
    

    //Pipeline
    void CreateVulkanPipeline(FVulkanEngine& engine, Pipeline::FPipelineConfigInfo& configInfo)
    {
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;

        auto bindingDescription = Types::Vertex::getBindingDescription();
        auto attributeDescriptions = Types::Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        /*vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        try
        {
            m_PipelineLayout = m_pDevice->createPipelineLayout(pipelineLayoutInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }*/

        vk::GraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.stageCount = engine.pipeline.vShaderBuffer.size();
        pipelineInfo.pStages = engine.pipeline.vShaderBuffer.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &configInfo.viewportInfo;
        pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
        pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
        pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
        pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
        pipelineInfo.pDynamicState = nullptr;

        pipelineInfo.layout = engine.pipeline.layout;
        pipelineInfo.renderPass = engine.swapchain.renderPass;
        pipelineInfo.subpass = configInfo.subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = nullptr;

        try
        {
            engine.pipeline.graphics = engine.device.logical->createGraphicsPipeline(nullptr, pipelineInfo).value;
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }
    }
    

    //SwapChain
    void CreateSwapChain(FVulkanEngine& engine)
    {
        SwapChainSupportDetails swapChainSupport = Device::QuerySwapChainSupport(engine);

        vk::SurfaceFormatKHR surfaceFormat = SwapChain::ChooseSwapSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR presentMode = SwapChain::ChooseSwapPresentMode(swapChainSupport.presentModes);
        engine.swapchain.extent = SwapChain::(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.maxImageCount;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.flags = vk::SwapchainCreateFlagsKHR();
        createInfo.surface = engine.device.surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = engine.swapchain.extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

        QueueFamilyIndices indices = Device::FindQueueFamilies(engine);
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

        try
        {
            engine.swapchain.swapChain = engine.device.logical->createSwapchainKHR(createInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        engine.swapchain.vImages = engine.device.logical->getSwapchainImagesKHR(engine.swapchain.swapChain);

        engine.swapchain.imageFormat = surfaceFormat.format;
    }
    
    void CreateSwapChainImageViews(FVulkanEngine& engine)
    {
        engine.swapchain.vImageViews.resize(engine.swapchain.vImages.size());

        for (size_t i = 0; i < engine.swapchain.vImages.size(); i++)
        {
            try
            {
                engine.swapchain.vImageViews[i] = Device::CreateImageView(engine, engine.swapchain.vImages[i], 1, engine.swapchain.imageFormat, vk::ImageAspectFlagBits::eColor);
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("Failed to create image views!");
            }
        }
    }
    
    void CreateDepthResources(FVulkanEngine& engine)
    {
        vk::Format depthFormat = Device::FindSupportedFormat(
        engine,
        {
            vk::Format::eD32Sfloat, 
            vk::Format::eD32SfloatS8Uint, 
            vk::Format::eD24UnormS8Uint
        },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);

        const uint32_t width = engine.swapchain.extent.width;
        const uint32_t height = engine.swapchain.extent.height;
        Device::CreateImage(engine, engine.swapchain.depth.image, engine.swapchain.depth.memory, width, height, 1,
        engine.device.samples, depthFormat, vk::ImageTiling::eOptimal, 
        vk::ImageUsageFlagBits::eDepthStencilAttachment,  vk::MemoryPropertyFlagBits::eDeviceLocal);

        engine.swapchain.depth.view = Device::CreateImageView(engine, engine.swapchain.depth.image, 1, depthFormat, vk::ImageAspectFlagBits::eDepth);

        Device::TransitionImageLayout(engine, engine.swapchain.depth.image, 1, vk::ImageAspectFlagBits::eDepth, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }

    void CreateMSAAResources(FVulkanEngine& engine)
    {
        vk::Format colorFormat = engine.swapchain.imageFormat;

        const uint32_t width = engine.swapchain.extent.width;
        const uint32_t height = engine.swapchain.extent.height;
        Device::CreateImage(engine, engine.swapchain.msaa.image, engine.swapchain.msaa.memory, width, height, 1, 
        engine.device.samples, colorFormat, vk::ImageTiling::eOptimal, 
        vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment, 
        vk::MemoryPropertyFlagBits::eDeviceLocal);

        engine.swapchain.msaa.view = Device::CreateImageView(engine, engine.swapchain.msaa.image, 1, colorFormat, vk::ImageAspectFlagBits::eColor);
    }
    
    void CreateRenderPass(FVulkanEngine& engine)
    {
        vk::AttachmentDescription colorAttachment = {};
        colorAttachment.format = engine.swapchain.imageFormat;
        colorAttachment.samples = engine.device.samples;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentDescription depthAttachment{};
        depthAttachment.format = Device::FindSupportedFormat(
        engine,
        {
            vk::Format::eD32Sfloat, 
            vk::Format::eD32SfloatS8Uint, 
            vk::Format::eD24UnormS8Uint
        },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);

        depthAttachment.samples = engine.device.samples;
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = engine.swapchain.imageFormat;
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

        try
        {
            engine.swapchain.renderPass = engine.device.logical->createRenderPass(renderPassInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }
    
    void CreateFramebuffers(FVulkanEngine& engine)
    {
        engine.swapchain.vFramebuffers.resize(engine.swapchain.vImageViews.size());

        for (size_t i = 0; i < engine.swapchain.vImageViews.size(); i++)
        {
            std::array<vk::ImageView, 3> attachments =
            {
                engine.swapchain.msaa.view,
                engine.swapchain.depth.view,
                engine.swapchain.vImageViews[i]
            };

            vk::FramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.renderPass = engine.swapchain.renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = engine.swapchain.extent.width;
            framebufferInfo.height = engine.swapchain.extent.height;
            framebufferInfo.layers = 1;

            try
            {
                engine.swapchain.vFramebuffers[i] = engine.device.logical->createFramebuffer(framebufferInfo);
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("Failed to create framebuffer!");
            }
        }
    }
    
    void CreateSyncObjects(FVulkanEngine& engine)
    {
        engine.swapchain.vImageAvailableSemaphores.resize(engine.swapchain.framesInFlight);
        engine.swapchain.vRenderFinishedSemaphores.resize(engine.swapchain.framesInFlight);
        engine.swapchain.vInFlightFences.resize(engine.swapchain.framesInFlight);

        try
        {
            for (size_t i = 0; i < engine.swapchain.framesInFlight; i++)
            {
                engine.swapchain.vImageAvailableSemaphores[i] = engine.device.logical->createSemaphore({});
                engine.swapchain.vRenderFinishedSemaphores[i] = engine.device.logical->createSemaphore({});
                engine.swapchain.vInFlightFences[i] = engine.device.logical->createFence({vk::FenceCreateFlagBits::eSignaled});
            }
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create synchronization objects for a frame!");
        }
    }
    
}