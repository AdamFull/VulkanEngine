#include "VulkanHighLevel.h"
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
    std::stringstream ss;
    ss << "DEBUG[";

    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: ss << "VERBOSE"; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: ss << "INFO"; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: ss << "WARNING"; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: ss << "ERROR"; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: break;
    }

    ss << "]: " << pCallbackData->pMessage;
    std::cerr << ss.str() << std::endl;

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

Device::~Device()
{
    Destroy(m_commandPool);

    // surface is created by glfw, therefore not using a Unique handle
    Destroy(m_surface);

    if (VulkanStaticHelper::m_bEnableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(m_vkInstance, m_vkDebugUtils, nullptr);
    }

    vkDestroyDevice(m_logical, nullptr);
    vkDestroyInstance(m_vkInstance, nullptr);
}

// TODO: add features picking while initialization
void Device::Create(const char *pApplicationName, uint32_t applicationVersion,
                    const char *pEngineName, uint32_t engineVersion,
                    uint32_t apiVersion)
{
    CreateInstance(pApplicationName, applicationVersion, pEngineName, engineVersion, apiVersion);
    CreateDebugCallback();
    CreateSurface();
    CreateDevice();
    CreateCommandPool();
}

uint32_t Device::GetVulkanVersion()
{
	if (m_vkInstance)
		return VK_API_VERSION_1_0;

	return 0;
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

    m_vkInstance = vk::createInstance(createInfo, nullptr);
    assert(m_vkInstance && "Vulkan instance was not created.");
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
    if (CreateDebugUtilsMessengerEXT(m_vkInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT *>(&createInfo), nullptr, &m_vkDebugUtils) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug callback!");
    }
}

void Device::CreateSurface()
{
    assert(m_vkInstance && "Unable to create surface, cause vulkan instance is not valid");
    UWinHandle->CreateWindowSurface(m_vkInstance, m_surface);
    assert(m_surface && "Surface creation failed");
}

void Device::CreateDevice()
{
    assert(m_vkInstance && "Unable to create ligical device, cause vulkan instance is not valid");
    m_physical = GetPhysicalDevice();
    assert(m_physical && "No avaliable physical devices. Check device dependencies.");

    m_msaaSamples = GetMaxUsableSampleCount();
    QueueFamilyIndices indices = FindQueueFamilies(m_physical);

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
    deviceFeatures.multiViewport = VK_TRUE;
    deviceFeatures.geometryShader = VK_TRUE;

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

    m_logical = m_physical.createDevice(createInfo);
    assert(m_logical && "Failed to create logical device.");

    m_qGraphicsQueue = m_logical.getQueue(indices.graphicsFamily.value(), 0);
    assert(m_qGraphicsQueue && "Failed while getting graphics queue.");
    m_qPresentQueue = m_logical.getQueue(indices.presentFamily.value(), 0);
    assert(m_qPresentQueue && "Failed while getting present queue.");
}

void Device::CreateCommandPool()
{
    assert(m_physical && "Cannot create command pool, cause physical device is not valid.");
    assert(m_logical && "Cannot create command pool, cause logical device is not valid.");
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_physical);

    vk::CommandPoolCreateInfo poolInfo = {};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    m_commandPool = m_logical.createCommandPool(poolInfo);
    assert(m_commandPool && "Failed while creating command pool");
}

uint32_t Device::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
    vk::PhysicalDeviceMemoryProperties memProperties = m_physical.getMemoryProperties();

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

        if (queueFamily.queueCount > 0 && device.getSurfaceSupportKHR(i, m_surface))
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
    return FindQueueFamilies(m_physical);
}

SwapChainSupportDetails Device::QuerySwapChainSupport(const vk::PhysicalDevice &device)
{
    SwapChainSupportDetails details;
    details.capabilities = device.getSurfaceCapabilitiesKHR(m_surface);
    details.formats = device.getSurfaceFormatsKHR(m_surface);
    details.presentModes = device.getSurfacePresentModesKHR(m_surface);

    return details;
}

SwapChainSupportDetails Device::QuerySwapChainSupport()
{
    return QuerySwapChainSupport(m_physical);
}

vk::SampleCountFlagBits Device::GetMaxUsableSampleCount()
{
    vk::PhysicalDeviceProperties physicalDeviceProperties;
    m_physical.getProperties(&physicalDeviceProperties);

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
    assert(m_logical && "Cannot create command buffer, cause logical device is not valid.");
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.commandPool = pool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = count;

    // TODO: Handle error
    return m_logical.allocateCommandBuffers(allocInfo);
}

std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>>
Device::CreateCommandBuffer(vk::CommandBufferLevel level, uint32_t count)
{
    return CreateCommandBuffer(level, m_commandPool, count);
}

vk::CommandBuffer Device::BeginSingleTimeCommands()
{
    assert(m_commandPool && "Cannot create command buffer, cause command pool is not valid.");
    vk::CommandBuffer commandBuffer = CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, m_commandPool, 1)[0];

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

    m_qGraphicsQueue.submit(submitInfo, nullptr);
    m_qGraphicsQueue.waitIdle();

    m_logical.freeCommandBuffers(m_commandPool, commandBuffer);
}

/*****************************************Image work helpers*****************************************/

void Device::CreateOnDeviceBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory)
{
    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    buffer = m_logical.createBuffer(bufferInfo);
    assert(buffer && "On device buffer was not created");

    vk::MemoryRequirements memRequirements = m_logical.getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo allocInfo = {};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    bufferMemory = m_logical.allocateMemory(allocInfo);
    assert(bufferMemory && "Buffer memory was not allocated");

    m_logical.bindBufferMemory(buffer, bufferMemory, 0);
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
    auto device = m_vkInstance.enumeratePhysicalDevices().front();
    if (device && IsDeviceSuitable(device))
    {
        return device;
    }
    return nullptr;
}

std::vector<vk::PhysicalDevice> Device::GetAvaliablePhysicalDevices()
{
    auto devices = m_vkInstance.enumeratePhysicalDevices();
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