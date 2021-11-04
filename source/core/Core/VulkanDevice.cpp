#include "VulkanDevice.h"
#include "Window/WindowHandle.h"
#include "VulkanStaticHelper.h"

using namespace Engine::Core;
using namespace Engine::Core::Window;

VkResult Device::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pCallback)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL Device::ValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void Device::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, callback, pAllocator);
    }
}

// TODO: add features picking while initialization
void Device::Create(std::unique_ptr<WindowHandle> &pWindow, const char *pApplicationName, uint32_t applicationVersion,
                    const char *pEngineName, uint32_t engineVersion,
                    uint32_t apiVersion)
{
    CreateInstance(pApplicationName, applicationVersion, pEngineName, engineVersion, apiVersion);
    CreateDebugCallback();
    CreateSurface(pWindow);
    CreateDevice();
    CreateCommandPool();
}

void Device::Cleanup()
{
    Destroy(data.commandPool);

    // surface is created by glfw, therefore not using a Unique handle
    Destroy(data.surface);

    if (VulkanStaticHelper::m_bEnableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(data.vkInstance.get(), data.vkDebugUtils, nullptr);
    }
}

void Device::CreateInstance(const char *pApplicationName, uint32_t applicationVersion,
                            const char *pEngineName, uint32_t engineVersion,
                            uint32_t apiVersion)
{
    if (VulkanStaticHelper::m_bEnableValidationLayers && !VulkanStaticHelper::CheckValidationLayerSupport())
    {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    auto appInfo = vk::ApplicationInfo{};
    appInfo.pApplicationName = pApplicationName;
    appInfo.applicationVersion = applicationVersion;
    appInfo.pEngineName = pEngineName;
    appInfo.engineVersion = engineVersion;
    appInfo.apiVersion = apiVersion;

    auto extensions = VulkanStaticHelper::GetRequiredExtensions();

    auto createInfo = vk::InstanceCreateInfo{};
    createInfo.flags = vk::InstanceCreateFlags();
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (VulkanStaticHelper::m_bEnableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanStaticHelper::m_vValidationLayers.size());
        createInfo.ppEnabledLayerNames = VulkanStaticHelper::m_vValidationLayers.data();
    }

    data.vkInstance = vk::createInstanceUnique(createInfo, nullptr);
    assert(data.vkInstance && "Vulkan instance was not created.");
}

void Device::CreateDebugCallback()
{
    if (!VulkanStaticHelper::m_bEnableValidationLayers)
        return;

    auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
        vk::DebugUtilsMessengerCreateFlagsEXT(),
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        ValidationCallback,
        nullptr);

    // NOTE: Vulkan-hpp has methods for this, but they trigger linking errors...
    if (CreateDebugUtilsMessengerEXT(*data.vkInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT *>(&createInfo), nullptr, &data.vkDebugUtils) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug callback!");
    }
}

void Device::CreateSurface(std::unique_ptr<WindowHandle> &pWindow)
{
    assert(data.vkInstance && "Unable to create surface, cause vulkan instance is not valid");
    pWindow->CreateWindowSurface(data.vkInstance, data.surface);
    assert(data.surface && "Surface creation failed");
}

void Device::CreateDevice()
{
    assert(data.vkInstance && "Unable to create ligical device, cause vulkan instance is not valid");
    data.physical = GetPhysicalDevice();
    assert(data.physical && "No avaliable physical devices. Check device dependencies.");

    data.msaaSamples = GetMaxUsableSampleCount();
    QueueFamilyIndices indices = FindQueueFamilies(data.physical);

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
    deviceFeatures.fillModeNonSolid = VK_TRUE;

    auto createInfo = vk::DeviceCreateInfo(
        vk::DeviceCreateFlags(),
        static_cast<uint32_t>(queueCreateInfos.size()),
        queueCreateInfos.data());

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(VulkanStaticHelper::m_vDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = VulkanStaticHelper::m_vDeviceExtensions.data();

    if (VulkanStaticHelper::m_bEnableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanStaticHelper::m_vValidationLayers.size());
        createInfo.ppEnabledLayerNames = VulkanStaticHelper::m_vValidationLayers.data();
    }

    data.logical = data.physical.createDeviceUnique(createInfo);
    assert(data.logical && "Failed to create logical device.");

    data.qGraphicsQueue = data.logical->getQueue(indices.graphicsFamily.value(), 0);
    assert(data.qGraphicsQueue && "Failed while getting graphics queue.");
    data.qPresentQueue = data.logical->getQueue(indices.presentFamily.value(), 0);
    assert(data.qPresentQueue && "Failed while getting present queue.");
}

void Device::CreateCommandPool()
{
    assert(data.physical && "Cannot create command pool, cause physical device is not valid.");
    assert(data.logical && "Cannot create command pool, cause logical device is not valid.");
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(data.physical);

    vk::CommandPoolCreateInfo poolInfo = {};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    data.commandPool = data.logical->createCommandPool(poolInfo);
    assert(data.commandPool && "Failed while creating command pool");
}

vk::Format Device::FindSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
    for (vk::Format format : candidates)
    {
        vk::FormatProperties props;
        data.physical.getFormatProperties(format, &props);

        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    // TODO: Handle null result
    throw std::runtime_error("Failed to find supported format!");
}

uint32_t Device::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
    vk::PhysicalDeviceMemoryProperties memProperties = data.physical.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}

QueueFamilyIndices Device::FindQueueFamilies(vk::PhysicalDevice device)
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

        if (queueFamily.queueCount > 0 && device.getSurfaceSupportKHR(i, data.surface))
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

QueueFamilyIndices Device::FindQueueFamilies()
{
    return FindQueueFamilies(data.physical);
}

SwapChainSupportDetails Device::QuerySwapChainSupport(const vk::PhysicalDevice &device)
{
    SwapChainSupportDetails details;
    details.capabilities = device.getSurfaceCapabilitiesKHR(data.surface);
    details.formats = device.getSurfaceFormatsKHR(data.surface);
    details.presentModes = device.getSurfacePresentModesKHR(data.surface);

    return details;
}

SwapChainSupportDetails Device::QuerySwapChainSupport()
{
    return QuerySwapChainSupport(data.physical);
}

vk::SampleCountFlagBits Device::GetMaxUsableSampleCount()
{
    vk::PhysicalDeviceProperties physicalDeviceProperties;
    data.physical.getProperties(&physicalDeviceProperties);

    vk::SampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & vk::SampleCountFlagBits::e64)
    {
        return vk::SampleCountFlagBits::e64;
    }
    if (counts & vk::SampleCountFlagBits::e32)
    {
        return vk::SampleCountFlagBits::e32;
    }
    if (counts & vk::SampleCountFlagBits::e16)
    {
        return vk::SampleCountFlagBits::e16;
    }
    if (counts & vk::SampleCountFlagBits::e8)
    {
        return vk::SampleCountFlagBits::e8;
    }
    if (counts & vk::SampleCountFlagBits::e4)
    {
        return vk::SampleCountFlagBits::e4;
    }
    if (counts & vk::SampleCountFlagBits::e2)
    {
        return vk::SampleCountFlagBits::e2;
    }

    return vk::SampleCountFlagBits::e1;
}

/*************************************Data transfer***************************************************/
std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>>
Device::CreateCommandBuffer(vk::CommandBufferLevel level, vk::CommandPool &pool, uint32_t count)
{
    assert(data.logical && "Cannot create command buffer, cause logical device is not valid.");
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.commandPool = pool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = count;

    // TODO: Handle error
    return data.logical->allocateCommandBuffers(allocInfo);
}

std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>>
Device::CreateCommandBuffer(vk::CommandBufferLevel level, uint32_t count)
{
    return CreateCommandBuffer(level, data.commandPool, count);
}

vk::CommandBuffer Device::BeginSingleTimeCommands()
{
    assert(data.commandPool && "Cannot create command buffer, cause command pool is not valid.");
    vk::CommandBuffer commandBuffer = CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, data.commandPool, 1)[0];

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void Device::EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
    commandBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.sType = vk::StructureType::eSubmitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    data.qGraphicsQueue.submit(submitInfo, nullptr);
    data.qGraphicsQueue.waitIdle();

    data.logical->freeCommandBuffers(data.commandPool, commandBuffer);
}

/*****************************************Image work helpers*****************************************/
void Device::CreateImage(vk::Image &image, vk::DeviceMemory &memory, vk::ImageCreateInfo createInfo, vk::MemoryPropertyFlags properties)
{
    assert(data.logical && "Cannot create image, cause logical device is not valid.");

    image = data.logical->createImage(createInfo, nullptr);
    assert(image && "Image was not created");

    vk::MemoryRequirements memReq{};
    data.logical->getImageMemoryRequirements(image, &memReq);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memReq.memoryTypeBits, properties);

    memory = data.logical->allocateMemory(allocInfo);
    assert(memory && "Image memory was not allocated");

    data.logical->bindImageMemory(image, memory, 0);
}

void Device::TransitionImageLayout(vk::Image &image, std::vector<vk::ImageMemoryBarrier> vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    for (auto &barrier : vBarriers)
    {
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.image = image;

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        }
        else
        {
            throw std::invalid_argument("Unsupported layout transition!");
        }
    }

    commandBuffer.pipelineBarrier(
        sourceStage,
        destinationStage,
        vk::DependencyFlags(),
        0, nullptr, 0, nullptr,
        static_cast<uint32_t>(vBarriers.size()), vBarriers.data());

    EndSingleTimeCommands(commandBuffer);
}

void Device::CopyBufferToImage(vk::Buffer &buffer, vk::Image &image, std::vector<vk::BufferImageCopy> vRegions)
{
    vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();
    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, static_cast<uint32_t>(vRegions.size()), vRegions.data());
    EndSingleTimeCommands(commandBuffer);
}

vk::ImageView Device::CreateImageView(vk::Image &pImage, vk::ImageViewCreateInfo viewInfo)
{
    assert(data.logical && "Cannot create image view, cause logical device is not valid.");
    viewInfo.image = pImage;

    vk::ImageView imageView;
    // TODO: Handle result
    auto result = data.logical->createImageView(&viewInfo, nullptr, &imageView);
    assert(imageView && "Was not created");

    return imageView;
}

void Device::CreateSampler(vk::Sampler &sampler, uint32_t mip_levels, vk::SamplerAddressMode eAddressMode)
{
    assert(data.physical && "Cannot create sampler, cause physical device is not valid.");
    assert(data.logical && "Cannot create sampler, cause logical device is not valid.");
    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.magFilter = vk::Filter::eLinear;
    samplerInfo.minFilter = vk::Filter::eLinear;
    samplerInfo.addressModeU = eAddressMode;
    samplerInfo.addressModeV = eAddressMode;
    samplerInfo.addressModeW = eAddressMode;

    vk::PhysicalDeviceProperties properties{};
    properties = data.physical.getProperties();

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = vk::CompareOp::eAlways;

    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(mip_levels);

    // TODO: Result handle
    auto result = data.logical->createSampler(&samplerInfo, nullptr, &sampler);
    assert(sampler && "Texture sampler was not created");
}

void Device::CreateOnDeviceBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory)
{
    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    buffer = data.logical->createBuffer(bufferInfo);
    assert(buffer && "On device buffer was not created");

    vk::MemoryRequirements memRequirements = data.logical->getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo allocInfo = {};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    bufferMemory = data.logical->allocateMemory(allocInfo);
    assert(bufferMemory && "Buffer memory was not allocated");

    data.logical->bindBufferMemory(buffer, bufferMemory, 0);
}

void Device::CopyOnDeviceBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
    vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

    vk::BufferCopy copyRegion = {};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);

    EndSingleTimeCommands(commandBuffer);
}

vk::PhysicalDevice Device::GetPhysicalDevice()
{
    auto device = data.vkInstance->enumeratePhysicalDevices().front();
    if (device && IsDeviceSuitable(device))
    {
        return device;
    }
    return nullptr;
}

std::vector<vk::PhysicalDevice> Device::GetAvaliablePhysicalDevices()
{
    auto devices = data.vkInstance->enumeratePhysicalDevices();
    std::vector<vk::PhysicalDevice> output_devices;
    if (devices.size() == 0)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    for (const auto &device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            output_devices.emplace_back(device);
        }
    }

    if (output_devices.size() == 0)
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
    return output_devices;
}

bool Device::IsDeviceSuitable(const vk::PhysicalDevice &device)
{
    QueueFamilyIndices indices = FindQueueFamilies(device);

    bool extensionsSupported = VulkanStaticHelper::CheckDeviceExtensionSupport(device);

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