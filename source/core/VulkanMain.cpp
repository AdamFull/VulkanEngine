#include "VulkanMain.h"
#include "WindowHandle.h"
#include "VulkanDevice.h"
#include "DataTypes/VulkanVertex.h"

namespace Engine::Main
{
    void CreateWindowHandle(FVulkanRenderInstance& renderInstance, int width, int height, const char* srWinName)
    {
        Window::Initialize(renderInstance, width, height, srWinName);
    }

    //Device
    void CreateInstance(FVulkanRenderInstance& renderInstance)
    {
        if (Device::bEnableValidationLayers && !Device::CheckValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }

        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName;
        appInfo.applicationVersion;
        appInfo.pEngineName;
        appInfo.engineVersion;
        appInfo.apiVersion;

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
            renderInstance.vkInstance = vk::createInstanceUnique(createInfo, nullptr);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create instance!");
        }
    }
    
    void SetupDebugMessenger(FVulkanRenderInstance& renderInstance)
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
    
    void CreateSurface(FVulkanRenderInstance& renderInstance)
    {
        Window::CreateWindowSurface(renderInstance);
    }
    
    void CreateLogicalDevice(FVulkanRenderInstance& renderInstance)
    {
        renderInstance.device.physical = Device::GetPhysicalDevice();
        //renderInstance.device.msaaSamples = Device::GetMaxUsableSampleCount();
        QueueFamilyIndices indices = Device::FindQueueFamilies(renderInstance);

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
            renderInstance.device.logical = renderInstance.device.physical.createDeviceUnique(createInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create logical device!");
        }

        renderInstance.device.graphicsQueue = renderInstance.device.logical->getQueue(indices.graphicsFamily.value(), 0);
        renderInstance.device.presentQueue = renderInstance.device.logical->getQueue(indices.presentFamily.value(), 0);
    }
    
    void CreateCommandPool(FVulkanRenderInstance& renderInstance)
    {
        QueueFamilyIndices queueFamilyIndices = Device::FindQueueFamilies(renderInstance);

        vk::CommandPoolCreateInfo poolInfo = {};
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        try
        {
            renderInstance.device.commandPool = renderInstance.device.logical->createCommandPool(poolInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create command pool!");
        }
    }
    

    //Pipeline
    void CreateVulkanPipeline(FVulkanRenderInstance& renderInstance, FPipelineConfigInfo& configInfo)
    {
        assert(configInfo.pipelineLayout && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
        assert(configInfo.renderPass && "Cannot create graphics pipeline: no renderPass provided in configInfo");

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
        pipelineInfo.stageCount = m_vShaderBuffer.size();
        pipelineInfo.pStages = m_vShaderBuffer.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &configInfo.viewportInfo;
        pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
        pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
        pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
        pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
        pipelineInfo.pDynamicState = nullptr;

        pipelineInfo.layout = renderInstance.pipeline.pipelineLayout;
        pipelineInfo.renderPass = renderInstance.device.renderPass;
        pipelineInfo.subpass = configInfo.subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = nullptr;

        try
        {
            renderInstance.pipeline.graphicsPipeline = renderInstance.device.logical->createGraphicsPipeline(nullptr, pipelineInfo).value;
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }
    }
    

    //SwapChain
    void CreateSwapChain(FVulkanRenderInstance& renderInstance)
    {
        SwapChainSupportDetails swapChainSupport = Device::QuerySwapChainSupport(renderInstance);

        vk::SurfaceFormatKHR surfaceFormat = VulkanStaticHelper::ChooseSwapSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR presentMode = VulkanStaticHelper::ChooseSwapPresentMode(swapChainSupport.presentModes);
        renderInstance.swapchain.extent = ChooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.maxImageCount;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.flags = vk::SwapchainCreateFlagsKHR();
        createInfo.surface = renderInstance.device.surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = renderInstance.swapchain.extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

        QueueFamilyIndices indices = Device::FindQueueFamilies(renderInstance);
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
            renderInstance.swapchain.swapChain = renderInstance.device.logical->createSwapchainKHR(createInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        renderInstance.swapchain.vImages = renderInstance.device.logical->getSwapchainImagesKHR(m_pSwapchain);

        renderInstance.swapchain.imageFormat = surfaceFormat.format;
    }
    
    void CreateSwapChainImageViews(FVulkanRenderInstance& renderInstance)
    {

    }
    
    void CreateDepthResources(FVulkanRenderInstance& renderInstance)
    {

    }
    
    void CreateRenderPass(FVulkanRenderInstance& renderInstance)
    {

    }
    
    void CreateFramebuffers(FVulkanRenderInstance& renderInstance)
    {

    }
    
    void CreateSyncObjects(FVulkanRenderInstance& renderInstance)
    {

    }
    
}