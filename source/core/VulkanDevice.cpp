#include "VulkanDevice.h"
#include "WindowHandle.h"

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

namespace Engine
{
    VulkanDevice::VulkanDevice(std::unique_ptr<WindowHandle>& window)
    {
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface(window);
        CreateLogicalDevice();
        CreateCommandPool();
    }

    VulkanDevice::~VulkanDevice()
    {
        m_pLogicalDevice->destroyCommandPool(m_CommandPool);
        

        if (bEnableValidationLayers) 
        {
            DestroyDebugUtilsMessengerEXT(m_pInstance.get(), debugMessenger, nullptr);
        }

        m_pInstance->destroySurfaceKHR(m_Surface);
    }

    SwapChainSupportDetails VulkanDevice::GetSwapChainSupport()
    {
        return QuerySwapChainSupport(m_pPhysicalDevice);
    }

    uint32_t VulkanDevice::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memProperties = m_pPhysicalDevice.getMemoryProperties();

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type!");
    }

    QueueFamilyIndices VulkanDevice::FindPhysicalQueueFamilies()
    {
        return FindQueueFamilies(m_pPhysicalDevice);
    }

    vk::Format VulkanDevice::FindSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
    {
        for (vk::Format format : candidates)
        {
            vk::FormatProperties props;
            m_pPhysicalDevice.getFormatProperties(format, &props);

            if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        //TODO: Handle null result
        throw std::runtime_error("Failed to find supported format!");
    }

    void VulkanDevice::CreateOnDeviceBuffer(vk::DeviceSize size, vk::BufferUsageFlags buffer_usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory)
    {
        vk::BufferCreateInfo bufferInfo = {};
        bufferInfo.size = size;
        bufferInfo.usage = buffer_usage;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        try
        {
            buffer = m_pLogicalDevice->createBuffer(bufferInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create buffer!");
        }

        vk::MemoryRequirements memRequirements = m_pLogicalDevice->getBufferMemoryRequirements(buffer);

        vk::MemoryAllocateInfo allocInfo = {};
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        try
        {
            bufferMemory = m_pLogicalDevice->allocateMemory(allocInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to allocate buffer memory!");
        }

        m_pLogicalDevice->bindBufferMemory(buffer, bufferMemory, 0);
    }

    std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> VulkanDevice::CreateCommandBuffer(vk::CommandBufferLevel level, vk::CommandPool& pool, uint32_t count)
    {
        vk::CommandBufferAllocateInfo allocInfo = {};
        allocInfo.commandPool = pool;
        allocInfo.level = level;
        allocInfo.commandBufferCount = count;

        //TODO: Handle error
        return m_pLogicalDevice->allocateCommandBuffers(allocInfo);
    }

    vk::CommandBuffer VulkanDevice::BeginSingleTimeCommands()
    {
        vk::CommandBuffer commandBuffer = CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, m_CommandPool, 1)[0];

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        commandBuffer.begin(beginInfo);

        return commandBuffer;
    }

    void VulkanDevice::EndSingleTimeCommands(vk::CommandBuffer& commandBuffer)
    {
        commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        m_qGraphicsQueue.submit(submitInfo, nullptr);
        m_qGraphicsQueue.waitIdle();

        m_pLogicalDevice->freeCommandBuffers(m_CommandPool, commandBuffer);
    }

    void VulkanDevice::CopyOnDeviceBuffer(vk::Buffer& srcBuffer, vk::Buffer& dstBuffer, vk::DeviceSize size)
    {
        vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

        vk::BufferCopy copyRegion = {};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);

        EndSingleTimeCommands(commandBuffer);
    }

    void VulkanDevice::CopyBufferToImage(vk::Buffer& buffer, vk::Image& image, uint32_t width, uint32_t height, uint32_t layerCount)
    {
        vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

        vk::BufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = vk::Offset3D{0, 0, 0};
        region.imageExtent = vk::Extent3D
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
            1
        };

        commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

        EndSingleTimeCommands(commandBuffer);
    }

    void VulkanDevice::CreateImage(vk::Image& image, vk::DeviceMemory& memory, uint32_t width, uint32_t height, uint32_t mip_levels, 
        vk::SampleCountFlagBits num_samples, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, 
        vk::MemoryPropertyFlags properties)
    {
        vk::ImageCreateInfo imageInfo{};
        imageInfo.imageType = vk::ImageType::e2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mip_levels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageInfo.usage = usage;
        imageInfo.sharingMode = vk::SharingMode::eExclusive;
        imageInfo.samples = num_samples;

        //TODO: Check valid
        image = m_pLogicalDevice->createImage(imageInfo, nullptr);

        vk::MemoryRequirements memReq{};
        m_pLogicalDevice->getImageMemoryRequirements(image, &memReq);

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memReq.memoryTypeBits, properties);

        memory = m_pLogicalDevice->allocateMemory(allocInfo);
        m_pLogicalDevice->bindImageMemory(image, memory, 0);
    }

    vk::ImageView VulkanDevice::CreateImageView(vk::Image& pImage, uint32_t mip_levels, vk::Format eFormat, vk::ImageAspectFlags aspectFlags)
    {
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.image = pImage;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = eFormat;
        viewInfo.subresourceRange.aspectMask = aspectFlags;;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mip_levels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vk::ImageView imageView;
        //TODO: Handle result
        auto result = m_pLogicalDevice->createImageView(&viewInfo, nullptr, &imageView);

        return imageView;
    }

    void VulkanDevice::CreateInstance()
    {
        if (bEnableValidationLayers && !CheckValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        vk::ApplicationInfo appInfo = {};
        appInfo.pApplicationName = "App";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        auto extensions = GetRequiredExtensions();

        auto createInfo = vk::InstanceCreateInfo(
            vk::InstanceCreateFlags(),
            &appInfo,
            0, nullptr,                                                 // enabled layers
            static_cast<uint32_t>(extensions.size()), extensions.data() // enabled extensions
        );

        if (bEnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }

        try
        {
            m_pInstance = vk::createInstanceUnique(createInfo, nullptr);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create instance!");
        }
    }

    void VulkanDevice::SetupDebugMessenger()
    {
        if (!bEnableValidationLayers)
            return;

        auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
            vk::DebugUtilsMessengerCreateFlagsEXT(),
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            ValidationCallback,
            nullptr);

        // NOTE: Vulkan-hpp has methods for this, but they trigger linking errors...
        if (CreateDebugUtilsMessengerEXT(m_pInstance.get(), reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, &debugMessenger) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to set up debug callback!");
        }
        //m_pVkDebugUtils = m_pVkInstance->createDebugUtilsMessengerEXTUnique(createInfo);
    }

    void VulkanDevice::CreateSurface(std::unique_ptr<WindowHandle>& window)
    {
        window->CreateWindowSurface(m_pInstance.get(), m_Surface);
    }

    vk::PhysicalDevice VulkanDevice::GetPhysicalDevice()
    {
        auto device = m_pInstance->enumeratePhysicalDevices().front();
        if(device && IsDeviceSuitable(device))
        {
            return device;
        }
        return nullptr;
    }

    void VulkanDevice::CreateLogicalDevice()
    {
        m_pPhysicalDevice = GetPhysicalDevice();
        //msaaSamples = GetMaxUsableSampleCount();
        QueueFamilyIndices indices = FindQueueFamilies(m_pPhysicalDevice);

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
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (bEnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }

        try
        {
            m_pLogicalDevice = m_pPhysicalDevice.createDeviceUnique(createInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create logical device!");
        }

        m_qGraphicsQueue = m_pLogicalDevice->getQueue(indices.graphicsFamily.value(), 0);
        m_qPresentQueue = m_pLogicalDevice->getQueue(indices.presentFamily.value(), 0);
    }

    void VulkanDevice::CreateCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_pPhysicalDevice);

        vk::CommandPoolCreateInfo poolInfo = {};
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        try
        {
            m_CommandPool = m_pLogicalDevice->createCommandPool(poolInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create command pool!");
        }
    }

    // helper functions
    bool VulkanDevice::IsDeviceSuitable(vk::PhysicalDevice device)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device);

        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        vk::PhysicalDeviceFeatures supportedFeatures{};
        supportedFeatures = device.getFeatures();

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy && supportedFeatures.sampleRateShading;
    }

    std::vector<const char *> VulkanDevice::GetRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (bEnableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool VulkanDevice::CheckValidationLayerSupport()
    {
        auto availableLayers = vk::enumerateInstanceLayerProperties();

        for (const char *layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto &layerProperties : availableLayers)
            {
                std::cout << layerProperties.layerName << std::endl;
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    QueueFamilyIndices VulkanDevice::FindQueueFamilies(vk::PhysicalDevice device)
    {
        QueueFamilyIndices indices;

        auto queueFamilies = device.getQueueFamilyProperties();

        int i = 0;
        for (const auto &queueFamily : queueFamilies)
        {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                indices.graphicsFamily = i;
            }

            if (queueFamily.queueCount > 0 && device.getSurfaceSupportKHR(i, m_Surface))
            {
                indices.presentFamily = i;
            }

            if (indices.isComplete())
            {
                break;
            }

            i++;
        }

        return indices;
    }

    void VulkanDevice::PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT &createInfo)
    {
    }

    bool VulkanDevice::CheckDeviceExtensionSupport(vk::PhysicalDevice device)
    {
        std::set<std::string> sRequiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto &extension : device.enumerateDeviceExtensionProperties())
        {
            sRequiredExtensions.erase(extension.extensionName);
        }

        return sRequiredExtensions.empty();
    }

    SwapChainSupportDetails VulkanDevice::QuerySwapChainSupport(vk::PhysicalDevice device)
    {
        SwapChainSupportDetails details;
        details.capabilities = device.getSurfaceCapabilitiesKHR(m_Surface);
        details.formats = device.getSurfaceFormatsKHR(m_Surface);
        details.presentModes = device.getSurfacePresentModesKHR(m_Surface);

        return details;
    }
}