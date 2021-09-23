#include "VulkanMain.h"
#include "WindowHandle.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "DataTypes/Special/VulkanVertex.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

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
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
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

        auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
            vk::DebugUtilsMessengerCreateFlagsEXT(),
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            ValidationCallback,
            nullptr);

        // NOTE: Vulkan-hpp has methods for this, but they trigger linking errors...
        if (CreateDebugUtilsMessengerEXT(engine.device.vkInstance.get(), reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, &engine.device.debugMessenger) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to set up debug callback!");
        }
    }
    
    void CreateSurface(FVulkanEngine& engine)
    {
        Window::CreateWindowSurface(engine);
    }
    
    void CreateLogicalDevice(FVulkanEngine& engine)
    {
        engine.device.physical = Device::GetPhysicalDevice(engine);
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
    void CreateDescriptorSetLayout(FVulkanEngine& engine)
    {
        vk::DescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

        vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
        vk::DescriptorSetLayoutCreateInfo createInfo{};
        createInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
        createInfo.pBindings = bindings.data();

        //TODO: check result
        auto result = engine.device.logical->createDescriptorSetLayout(&createInfo, nullptr, &engine.pipeline.descriptorSetLayout);
    }

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

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        vk::Viewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)engine.swapchain.extent.width;
        viewport.height = (float)engine.swapchain.extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vk::Rect2D scissor = {};
        scissor.offset = vk::Offset2D{0, 0};
        scissor.extent = engine.swapchain.extent;

        vk::PipelineViewportStateCreateInfo viewportState = {};
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        vk::PipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
        rasterizer.depthBiasEnable = VK_FALSE;

        vk::PipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sampleShadingEnable = VK_TRUE;
        multisampling.minSampleShading = .2f;
        multisampling.rasterizationSamples = engine.device.samples;

        vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;

        vk::PipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = vk::LogicOp::eCopy;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        vk::PipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = vk::CompareOp::eLess;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &engine.pipeline.descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        try
        {
            engine.pipeline.layout = engine.device.logical->createPipelineLayout(pipelineLayoutInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        vk::GraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.stageCount = engine.pipeline.vShaderBuffer.size();
        pipelineInfo.pStages = engine.pipeline.vShaderBuffer.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.layout = engine.pipeline.layout;
        pipelineInfo.renderPass = engine.swapchain.renderPass;
        pipelineInfo.subpass = 0;
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
        engine.swapchain.extent = SwapChain::ChooseSwapExtent(engine, swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.maxImageCount;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo(
            vk::SwapchainCreateFlagsKHR(),
            engine.device.surface,
            imageCount,
            surfaceFormat.format,
            surfaceFormat.colorSpace,
            engine.swapchain.extent,
            1, // imageArrayLayers
            vk::ImageUsageFlagBits::eColorAttachment);

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

    void CreateUniformBuffers(FVulkanEngine& engine)
    {
        vk::DeviceSize bufferSize = sizeof(Types::UniformBufferObject);

        engine.swapchain.vUniformBuffers.resize(engine.swapchain.vImages.size());
        engine.swapchain.vUniformBuffersMemory.resize(engine.swapchain.vImages.size());

        for (size_t i = 0; i < engine.swapchain.vImages.size(); i++) 
        {
            Device::CreateOnDeviceBuffer
            (
                engine,
                bufferSize, 
                vk::BufferUsageFlagBits::eUniformBuffer, 
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, 
                engine.swapchain.vUniformBuffers[i], 
                engine.swapchain.vUniformBuffersMemory[i]
            );
        }
    }

    void CreateDescriptorPool(FVulkanEngine& engine)
    {
        std::array<vk::DescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(engine.swapchain.vImages.size());
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(engine.swapchain.vImages.size());

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(engine.swapchain.vImages.size());

        //TODO:: add result checking
        engine.pipeline.descriptorPool = engine.device.logical->createDescriptorPool(poolInfo);
    }

    void CreateDescriptorSets(FVulkanEngine& engine)
    {
        std::vector<vk::DescriptorSetLayout> vDescriptorSetLayouts(engine.swapchain.vImages.size(), engine.pipeline.descriptorSetLayout);

        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = engine.pipeline.descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(engine.swapchain.vImages.size());
        allocInfo.pSetLayouts = vDescriptorSetLayouts.data();

        engine.pipeline.vDescriptorSets.resize(engine.swapchain.vImages.size());

        auto result = engine.device.logical->allocateDescriptorSets(&allocInfo, engine.pipeline.vDescriptorSets.data());

        for (size_t i = 0; i < engine.swapchain.vImages.size(); i++)
        {
            vk::DescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = engine.swapchain.vUniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(Types::UniformBufferObject);

            std::array<vk::WriteDescriptorSet, 2> descriptorWrites{};
            descriptorWrites[0].dstSet = engine.pipeline.vDescriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].dstSet = engine.pipeline.vDescriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo;

            engine.device.logical->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void CreateCommandBuffers(FVulkanEngine& engine)
    {
        engine.device.vCommandBuffer.resize(engine.swapchain.vFramebuffers.size());

        try
        {
            engine.device.vCommandBuffer = Device::CreateCommandBuffer(engine, vk::CommandBufferLevel::ePrimary, engine.device.commandPool, (uint32_t)engine.device.vCommandBuffer.size());
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to allocate command buffers!");
        }

        for (size_t i = 0; i < engine.device.vCommandBuffer.size(); i++)
        {
            vk::CommandBufferBeginInfo beginInfo = {};
            beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

            try
            {
                engine.device.vCommandBuffer[i].begin(beginInfo);
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("Failed to begin recording command buffer!");
            }

            vk::RenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.renderPass = engine.swapchain.renderPass;
            renderPassInfo.framebuffer = engine.swapchain.vFramebuffers[i];
            renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
            renderPassInfo.renderArea.extent = engine.swapchain.extent;

            std::array<vk::ClearValue, 2> clearValues{};
            clearValues[0].color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
            clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            engine.device.vCommandBuffer[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

            engine.device.vCommandBuffer[i].bindPipeline(vk::PipelineBindPoint::eGraphics, engine.pipeline.graphics);

            vk::Buffer vertexBuffers[] = {m_pVertexBuffer};
            vk::DeviceSize offsets[] = {0};
            engine.device.vCommandBuffer[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
            engine.device.vCommandBuffer[i].bindIndexBuffer(m_pIndexBuffer, 0, vk::IndexType::eUint32);

            engine.device.vCommandBuffer[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, engine.pipeline.layout, 0, 1, &engine.pipeline.vDescriptorSets[i], 0, nullptr);

            engine.device.vCommandBuffer[i].drawIndexed(static_cast<uint32_t>(m_vIndices.size()), 1, 0, 0, 0);

            engine.device.vCommandBuffer[i].endRenderPass();

            try
            {
                engine.device.vCommandBuffer[i].end();
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("Failed to record command buffer!");
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

    void CleanupSyncObjects(FVulkanEngine& engine)
    {
        for (size_t i = 0; i < engine.swapchain.framesInFlight; i++)
        {
            engine.device.logical->destroySemaphore(engine.swapchain.vRenderFinishedSemaphores[i]);
            engine.device.logical->destroySemaphore(engine.swapchain.vImageAvailableSemaphores[i]);
            engine.device.logical->destroyFence(engine.swapchain.vInFlightFences[i]);
        }
    }
}