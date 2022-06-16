#include "util/ulog.hpp"
#include "VulkanHighLevel.h"
#include "VulkanStaticHelper.h"
#include "Resources/Materials/MaterialLoader.h"

using namespace engine::core;
using namespace engine::core::window;
using namespace engine::resources::material;

template<>
scope_ptr<CDevice> utl::singleton<CDevice>::_instance{nullptr};

std::vector<const char*> str_vector_to_char_ptr_vector(const std::vector<std::string>& from)
{
    std::vector<const char*> output{};
    for(auto& str : from)
    {
        auto size = str.size();
        char* data{nullptr};
        data = static_cast<char*>(calloc(size, sizeof(char)));
        memcpy(data, str.data(), size);
        data[size] = '\0';
        output.push_back(static_cast<const char*>(data));
    }
    return output;
}

VkResult CDevice::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pCallback)
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

VKAPI_ATTR VkBool32 VKAPI_CALL CDevice::validationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    utl::ELogLevel eType;

    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: eType = utl::ELogLevel::eVerbose; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: eType = utl::ELogLevel::eInfo; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: eType = utl::ELogLevel::eWarning; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: eType = utl::ELogLevel::eError; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: break;
    }

    utl::logger::log(eType, pCallbackData->pMessage);

    return VK_FALSE;
}

void CDevice::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, callback, pAllocator);
    }
}

void* allocationFunction(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void freeFunction(void* pUserData, void* pMemory)
{   
    free(pMemory);
}

void* reallocationFunction(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{    
    return realloc(pOriginal, size);
}

void internalAllocationNotification(void* pUserData, size_t  size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
}

void internalFreeNotification(void* pUserData, size_t size, VkInternalAllocationType  allocationType, VkSystemAllocationScope allocationScope)
{
}

static vk::AllocationCallbacks *createPAllocator()
{
    vk::AllocationCallbacks *m_allocator = (vk::AllocationCallbacks *)malloc(sizeof(vk::AllocationCallbacks));
    memset(m_allocator, 0, sizeof(vk::AllocationCallbacks));
    m_allocator->pfnAllocation = (PFN_vkAllocationFunction)(&allocationFunction);
    m_allocator->pfnReallocation = (PFN_vkReallocationFunction)(&reallocationFunction);
    m_allocator->pfnFree = (PFN_vkFreeFunction)&freeFunction;
    m_allocator->pfnInternalAllocation = (PFN_vkInternalAllocationNotification)&internalAllocationNotification;
    m_allocator->pfnInternalFree = (PFN_vkInternalFreeNotification)&internalFreeNotification;
    return m_allocator;
}

// TODO: add features picking while initialization
void CDevice::create(const FDeviceCreateInfo& deviceCI)
{
    pAllocator = createPAllocator();
    m_bValidation = deviceCI.validation;
    createInstance(deviceCI);
    createDebugCallback(deviceCI);
    createSurface();
    createDevice(deviceCI);
    createPipelineCache();
    createSwapchain();

    viewportExtent = swapchainExtent;
}

void CDevice::cleanup()
{
    cleanupSwapchain();
    destroy(&pipelineCache);
    
    commandPools.clear();
    
    // surface is created by glfw, therefore not using a Unique handle
    destroy(&vkSurface);

    destroy(&vkDevice);

    if (m_bValidation)
        destroyDebugUtilsMessengerEXT(vkInstance, m_vkDebugUtils, (const VkAllocationCallbacks*)pAllocator);
    destroy(&vkInstance);
}

void CDevice::tryRebuildSwapchain()
{
    if(bSwapChainRebuild)
    {
        vkDevice.waitIdle();
        commandPools.clear();
        cleanupSwapchain();
        createSwapchain();
        currentFrame = 0;
        viewportExtent = swapchainExtent;
        bSwapChainRebuild = false;
    }
}

void CDevice::updateCommandPools()
{
    for (auto it = commandPools.begin(); it != commandPools.end();) 
    {
		if ((*it).second.use_count() <= 1) 
        {
			it = commandPools.erase(it);
			continue;
		}
		++it;
	}
}

vk::Result CDevice::acquireNextImage(uint32_t *imageIndex)
{
    vk::Result res = vkDevice.waitForFences(1, &vInFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
    assert(res == vk::Result::eSuccess && "Waiting for fences error.");
    res = vkDevice.acquireNextImageKHR(swapChain, std::numeric_limits<uint64_t>::max(), vImageAvailableSemaphores[currentFrame], nullptr, imageIndex);
    return res;
}

vk::Result CDevice::submitCommandBuffers(const vk::CommandBuffer *commandBuffer, uint32_t *imageIndex, vk::QueueFlagBits queueBit)
{
    vk::Result res;
    vk::SubmitInfo submitInfo = {};
    vk::Semaphore waitSemaphores[] = {vImageAvailableSemaphores[currentFrame]};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffer;

    vk::Semaphore signalSemaphores[] = {vRenderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    res = vkDevice.resetFences(1, &vInFlightFences[currentFrame]);
    assert(res == vk::Result::eSuccess && "Cannot reset fences.");

    try
    {
        vk::Queue queue{};
        switch (queueBit)
        {
        case vk::QueueFlagBits::eGraphics: {
            queue = m_qGraphicsQueue;
        } break;
        case vk::QueueFlagBits::eCompute: {
            queue = m_qComputeQueue;
        } break;
        case vk::QueueFlagBits::eTransfer: {
            queue = m_qTransferQueue;
        } break;
        }
        queue.submit(submitInfo, vInFlightFences[currentFrame]);
    }
    catch (vk::SystemError err)
    {
        //throw std::runtime_error("failed to submit draw command buffer!");
    }

    vk::PresentInfoKHR presentInfo = {};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    vk::SwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = imageIndex;

    res = m_qPresentQueue.presentKHR(presentInfo);
    assert(res == vk::Result::eSuccess && "Failed to present KHR.");

    currentFrame = (currentFrame + 1) % framesInFlight;

    return res;
}

uint32_t CDevice::getVulkanVersion()
{
	if (vkInstance)
		return VK_API_VERSION_1_0;

	return 0;
}

void CDevice::createInstance(const FDeviceCreateInfo& deviceCI)
{
    if (deviceCI.validation && !VulkanStaticHelper::checkValidationLayerSupport(deviceCI.validationLayers))
    {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    auto extensions = VulkanStaticHelper::getRequiredExtensions(deviceCI.validation);

    vk::InstanceCreateInfo createInfo{};
    createInfo.pApplicationInfo = &deviceCI.appInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    std::array<vk::ValidationFeatureEnableEXT, 1> validationExt{vk::ValidationFeatureEnableEXT::eSynchronizationValidation};
    vk::ValidationFeaturesEXT validationFeatures{};
    validationFeatures.enabledValidationFeatureCount = validationExt.size();
    validationFeatures.pEnabledValidationFeatures = validationExt.data();
    if (deviceCI.validation)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(deviceCI.validationLayers.size());
        createInfo.ppEnabledLayerNames = deviceCI.validationLayers.data();
        createInfo.pNext = &validationFeatures;
    }

    vk::Result res = create(createInfo, &vkInstance);
    assert(res == vk::Result::eSuccess && "Cannot create vulkan instance.");
}

void CDevice::createDebugCallback(const FDeviceCreateInfo& deviceCI)
{
    if (!deviceCI.validation)
        return;

    auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
        vk::DebugUtilsMessengerCreateFlagsEXT(),
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        validationCallback,
        nullptr);

    // NOTE: Vulkan-hpp has methods for this, but they trigger linking errors...
    if (createDebugUtilsMessengerEXT(vkInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT *>(&createInfo), (const VkAllocationCallbacks*)pAllocator, &m_vkDebugUtils) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug callback!");
    }
}

void CDevice::createSurface()
{
    assert(vkInstance && "Unable to create surface, cause vulkan instance is not valid");
    VkSurfaceKHR rawSurfaceKHR{VK_NULL_HANDLE};
    CWindowHandle::inst()->createWindowSurface(vkInstance, (const void*)pAllocator, rawSurfaceKHR);
    vkSurface = rawSurfaceKHR;
    assert(vkSurface && "Surface creation failed");
}

void CDevice::createDevice(const FDeviceCreateInfo& deviceCI)
{
    assert(vkInstance && "Unable to create ligical device, cause vulkan instance is not valid");
    vkPhysical = getPhysicalDevice(deviceCI.deviceExtensions);
    assert(vkPhysical && "No avaliable physical devices. Check device dependencies.");

    if(isSupportedSampleCount(deviceCI.graphics.multisampling))
        m_msaaSamples = deviceCI.graphics.multisampling;
    else
        m_msaaSamples = vk::SampleCountFlagBits::e1;

    QueueFamilyIndices indices = findQueueFamilies(vkPhysical);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        queueCreateInfos.push_back({vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority});
    }

    auto createInfo = vk::DeviceCreateInfo{};
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceCI.deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceCI.deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceCI.deviceExtensions.data();

    if (deviceCI.validation)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(deviceCI.validationLayers.size());
        createInfo.ppEnabledLayerNames = deviceCI.validationLayers.data();
    }

    vk::Result res = create(createInfo, &vkDevice);
    assert(res == vk::Result::eSuccess && "Failed to create logical device.");

    m_qGraphicsQueue = vkDevice.getQueue(indices.graphicsFamily.value(), 0);
    assert(m_qGraphicsQueue && "Failed while getting graphics queue.");
    m_qPresentQueue = vkDevice.getQueue(indices.presentFamily.value(), 0);
    assert(m_qPresentQueue && "Failed while getting present queue.");
    m_qComputeQueue = vkDevice.getQueue(indices.computeFamily.value(), 0);
    assert(m_qComputeQueue && "Failed while getting compute queue.");
    m_qTransferQueue = vkDevice.getQueue(indices.transferFamily.value(), 0);
    assert(m_qTransferQueue && "Failed while getting transfer queue.");
}

void CDevice::createPipelineCache()
{
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    vk::Result res = create(pipelineCacheCreateInfo, &pipelineCache);
    assert(res == vk::Result::eSuccess && "Cannot create pipeline cache.");
}

void CDevice::createSwapchain()
{
    vk::Result res;
    vk::SwapchainKHR oldSwapchain = swapChain;
    swapChain = VK_NULL_HANDLE;

    SwapChainSupportDetails swapChainSupport = querySwapChainSupport();
    vk::SurfaceFormatKHR surfaceFormat = VulkanStaticHelper::chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = VulkanStaticHelper::chooseSwapPresentMode(swapChainSupport.presentModes);
    swapchainExtent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = framesInFlight;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.surface = vkSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = swapchainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    QueueFamilyIndices indices = findQueueFamilies();
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
    createInfo.oldSwapchain = oldSwapchain;

    res = create(createInfo, &swapChain);
    assert(res == vk::Result::eSuccess && "Swap chain was not created");

    vImages.resize(imageCount);
    res = vkDevice.getSwapchainImagesKHR(swapChain, &imageCount, vImages.data());
    assert(res == vk::Result::eSuccess && "Swap chain images was not created");

    imageFormat = surfaceFormat.format;

    if(oldSwapchain)
        destroy(&oldSwapchain);

    //Creating image views
    {
        vImageViews.resize(framesInFlight);
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = imageFormat;
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.components.r = vk::ComponentSwizzle::eR;
        viewInfo.components.g = vk::ComponentSwizzle::eG;
        viewInfo.components.b = vk::ComponentSwizzle::eB;
        viewInfo.components.a = vk::ComponentSwizzle::eA;

        for (size_t i = 0; i < imageCount; i++)
        {
            viewInfo.image = vImages[i];
            res = create(viewInfo, &vImageViews[i]);
            assert(res == vk::Result::eSuccess && "Cannot create image view");
        }
    }

    //Create sync objects
    {
        vImageAvailableSemaphores.resize(imageCount);
        vRenderFinishedSemaphores.resize(imageCount);
        vInFlightFences.resize(imageCount);

        try
        {
            vk::SemaphoreCreateInfo semaphoreCI{};
            vk::FenceCreateInfo fenceCI{};
            fenceCI.flags = vk::FenceCreateFlagBits::eSignaled;
            for (size_t i = 0; i < imageCount; i++)
            {
                res = create(semaphoreCI, &vImageAvailableSemaphores[i]);
                assert(res == vk::Result::eSuccess && "Cannot create semaphore");
                res = create(semaphoreCI, &vRenderFinishedSemaphores[i]);
                assert(res == vk::Result::eSuccess && "Cannot create semaphore");
                res = create(fenceCI, &vInFlightFences[i]);
                assert(res == vk::Result::eSuccess && "Cannot create fence");
            }
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create synchronization objects for a frame!");
        }
    }
}

void CDevice::cleanupSwapchain()
{
    for(size_t i = 0; i < framesInFlight; i++)
    {
        destroy(&vInFlightFences[i]);
        destroy(&vImageViews[i]);
        destroy(&vImageAvailableSemaphores[i]);
        destroy(&vRenderFinishedSemaphores[i]);
    }
    vInFlightFences.clear();
    vImageViews.clear();
    vImageAvailableSemaphores.clear();
    vRenderFinishedSemaphores.clear();
}

uint32_t CDevice::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
    vk::PhysicalDeviceMemoryProperties memProperties = vkPhysical.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}

QueueFamilyIndices CDevice::findQueueFamilies(vk::PhysicalDevice device)
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

        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eCompute)
        {
            indices.computeFamily = i;
        }

        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)
        {
            indices.transferFamily = i;
        }

        if (queueFamily.queueCount > 0 && device.getSurfaceSupportKHR(i, vkSurface))
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

QueueFamilyIndices CDevice::findQueueFamilies()
{
    return findQueueFamilies(vkPhysical);
}

SwapChainSupportDetails CDevice::querySwapChainSupport(const vk::PhysicalDevice &device)
{
    SwapChainSupportDetails details;
    details.capabilities = device.getSurfaceCapabilitiesKHR(vkSurface);
    details.formats = device.getSurfaceFormatsKHR(vkSurface);
    details.presentModes = device.getSurfacePresentModesKHR(vkSurface);

    return details;
}

SwapChainSupportDetails CDevice::querySwapChainSupport()
{
    return querySwapChainSupport(vkPhysical);
}

vk::Extent2D CDevice::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities)
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

std::vector<vk::SampleCountFlagBits> CDevice::getAvaliableSampleCount()
{
    vk::PhysicalDeviceProperties physicalDeviceProperties;
    vkPhysical.getProperties(&physicalDeviceProperties);
    std::vector<vk::SampleCountFlagBits> avaliableSamples{vk::SampleCountFlagBits::e1};

    vk::SampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & vk::SampleCountFlagBits::e64)
        avaliableSamples.push_back(vk::SampleCountFlagBits::e64);
    if (counts & vk::SampleCountFlagBits::e32)
        avaliableSamples.push_back(vk::SampleCountFlagBits::e32);
    if (counts & vk::SampleCountFlagBits::e16)
        avaliableSamples.push_back(vk::SampleCountFlagBits::e16);
    if (counts & vk::SampleCountFlagBits::e8)
        avaliableSamples.push_back(vk::SampleCountFlagBits::e8);   
    if (counts & vk::SampleCountFlagBits::e4)
        avaliableSamples.push_back(vk::SampleCountFlagBits::e4);
    if (counts & vk::SampleCountFlagBits::e2)
        avaliableSamples.push_back(vk::SampleCountFlagBits::e2);

    return avaliableSamples;
}

bool CDevice::isSupportedSampleCount(vk::SampleCountFlagBits samples)
{
    auto avaliableSamples = getAvaliableSampleCount();
    auto found = std::find(avaliableSamples.begin(), avaliableSamples.end(), samples);
    return found != avaliableSamples.end();
}

/*****************************************Image work helpers*****************************************/

void CDevice::copyOnDeviceBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eTransfer);
    auto commandBuffer = cmdBuf.getCommandBuffer();

    vk::BufferCopy copyRegion = {};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);

    cmdBuf.submitIdle();
}

const ref_ptr<CCommandPool>& CDevice::getCommandPool(const std::thread::id &threadId)
{
    auto it = commandPools.find(threadId);
    if(it != commandPools.end())
		return it->second;

    commandPools.emplace(threadId, make_ref<CCommandPool>(threadId));
	return commandPools[threadId];
}

vk::Extent2D CDevice::getExtent(bool automatic)
{
    auto engineMode = UHLInstance->getCI().engine.mode;
    if(automatic)
    {
        switch (engineMode)
        {
        case ELaunchMode::eEditor: return viewportExtent; break;
        case ELaunchMode::eGame: return swapchainExtent; break;
        }
    }
    return swapchainExtent;
}

float CDevice::getAspect(bool automatic)
{
    auto ext = getExtent(automatic);
    return static_cast<float>(ext.width) / static_cast<float>(ext.height);
}

void CDevice::setViewportExtent(vk::Extent2D extent)
{
    if(extent != viewportExtent)
    {
        bViewportRebuild = true;
        viewportExtent = extent;
    }
    else
        bViewportRebuild = false;
}

vk::PhysicalDevice CDevice::getPhysicalDevice(const std::vector<const char*>& deviceExtensions)
{
    auto device = vkInstance.enumeratePhysicalDevices().front();
    if (device && isDeviceSuitable(device, deviceExtensions))
    {
        return device;
    }
    return nullptr;
}

std::vector<vk::PhysicalDevice> CDevice::getAvaliablePhysicalDevices(const std::vector<const char*>& deviceExtensions)
{
    auto devices = vkInstance.enumeratePhysicalDevices();
    std::vector<vk::PhysicalDevice> output_devices;
    if (devices.size() == 0)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    for (const auto &device : devices)
    {
        if (isDeviceSuitable(device, deviceExtensions))
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

bool CDevice::isDeviceSuitable(const vk::PhysicalDevice &device, const std::vector<const char*>& deviceExtensions)
{
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = VulkanStaticHelper::checkDeviceExtensionSupport(device, deviceExtensions);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    vk::PhysicalDeviceFeatures supportedFeatures = device.getFeatures();

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy && supportedFeatures.sampleRateShading;
}

namespace vk
{
    NLOHMANN_JSON_SERIALIZE_ENUM
    (
        SampleCountFlagBits,
        {
            {SampleCountFlagBits::e1, "1"},
            {SampleCountFlagBits::e2, "2"},
            {SampleCountFlagBits::e4, "4"},
            {SampleCountFlagBits::e8, "8"},
            {SampleCountFlagBits::e16, "16"},
            {SampleCountFlagBits::e32, "32"},
            {SampleCountFlagBits::e64, "64"}
        }
    )

    void to_json(nlohmann::json &json, const ApplicationInfo &type)
    {
        std::string appName{type.pApplicationName}, engineName{type.pEngineName};
        json = nlohmann::json{
            {"app_name", appName},
            {"app_version", type.applicationVersion},
            {"engine_name", engineName},
            {"engine_version", type.engineVersion},
            {"api_version", type.apiVersion}};
    }

    void from_json(const nlohmann::json &json, ApplicationInfo &type)
    {
        std::string appName, engineName;
        ParseArgument(json, appName, "app_name", true);
        type.pApplicationName = appName.data();
        ParseArgument(json, type.applicationVersion, "app_version", true);
        ParseArgument(json, engineName, "engine_name", true);
        type.pEngineName = engineName.data();
        ParseArgument(json, type.engineVersion, "engine_version", true);
        ParseArgument(json, type.apiVersion, "api_version", true);
    }

    void to_json(nlohmann::json &json, const PhysicalDeviceFeatures &type)
    {
        json = nlohmann::json{
            {"robust_buffer_access", type.robustBufferAccess},
            {"full_draw_index_uint32", type.fullDrawIndexUint32},
            {"image_cube_array", type.imageCubeArray},
            {"independent_blend", type.independentBlend},
            {"geometry_shader", type.geometryShader},
            {"tesselation_shader", type.tessellationShader},
            {"sample_rate_shading", type.sampleRateShading},
            {"dial_src_blend", type.dualSrcBlend},
            {"logic_op", type.logicOp},
            {"multi_draw_indirect", type.multiDrawIndirect},
            {"draw_indirect_first_instance", type.drawIndirectFirstInstance},
            {"depth_clamp", type.depthClamp},
            {"depth_bias_clamp", type.depthBiasClamp},
            {"full_mode_non_solid", type.fillModeNonSolid},
            {"depth_bounds", type.depthBounds},
            {"wide_lines", type.wideLines},
            {"large_points", type.largePoints},
            {"alpha_to_one", type.alphaToOne},
            {"multi_viewport", type.multiViewport},
            {"sampler_anisotropy", type.samplerAnisotropy},
            {"texture_compression_etc2", type.textureCompressionETC2},
            {"texture_compression_astc_ldr", type.textureCompressionASTC_LDR},
            {"texture_compression_bc", type.textureCompressionBC},
            {"occlusion_query_precise", type.occlusionQueryPrecise},
            {"pipeline_statistics_query", type.pipelineStatisticsQuery},
            {"vertex_pipeline_stores_and_atomics", type.vertexPipelineStoresAndAtomics},
            {"fragment_stores_and_atomics", type.fragmentStoresAndAtomics},
            {"shader_tessellation_and_geometry_point_size", type.shaderTessellationAndGeometryPointSize},
            {"shader_image_gather_extended", type.shaderImageGatherExtended},
            {"shader_storage_image_extended_formats", type.shaderStorageImageExtendedFormats},
            {"shader_storage_image_multisample", type.shaderStorageImageMultisample},
            {"shader_storage_image_read_without_format", type.shaderStorageImageReadWithoutFormat},
            {"shader_storage_image_write_without_format", type.shaderStorageImageWriteWithoutFormat},
            {"shader_uniform_buffer_array_dynamic_indexing", type.shaderUniformBufferArrayDynamicIndexing},
            {"shader_sampled_image_array_dynamic_indexing", type.shaderSampledImageArrayDynamicIndexing},
            {"shader_storage_buffer_array_dynamic_indexing", type.shaderStorageBufferArrayDynamicIndexing},
            {"shader_storage_image_array_dynamic_indexing", type.shaderStorageImageArrayDynamicIndexing},
            {"shader_clip_distance", type.shaderClipDistance},
            {"shader_cull_distance", type.shaderCullDistance},
            {"shader_float64", type.shaderFloat64},
            {"shader_int64", type.shaderInt64},
            {"shader_int16", type.shaderInt16},
            {"shader_resource_residency", type.shaderResourceResidency},
            {"shader_resource_min_log", type.shaderResourceMinLod},
            {"sparse_binding", type.sparseBinding},
            {"sparse_residency_buffer", type.sparseResidencyBuffer},
            {"sparse_residency_image_2d", type.sparseResidencyImage2D},
            {"sparse_residency_image_3d", type.sparseResidencyImage3D},
            {"sparse_residency_2_samples", type.sparseResidency2Samples},
            {"sparse_residency_4_samples", type.sparseResidency4Samples},
            {"sparse_residency_8_samples", type.sparseResidency8Samples},
            {"sparse_residency_16_samples", type.sparseResidency16Samples},
            {"sparse_residency_aliased", type.sparseResidencyAliased},
            {"variable_multisample_rate", type.variableMultisampleRate},
            {"full_mode_non_soloid", type.fillModeNonSolid},
            {"inherited_queries", type.inheritedQueries}};
    }

    void from_json(const nlohmann::json &json, PhysicalDeviceFeatures &type)
    { 
        ParseArgument(json, type.robustBufferAccess, "robust_buffer_access");
        ParseArgument(json, type.fullDrawIndexUint32, "full_draw_index_uint32");
        ParseArgument(json, type.imageCubeArray, "image_cube_array");
        ParseArgument(json, type.independentBlend, "independent_blend");
        ParseArgument(json, type.geometryShader, "geometry_shader");
        ParseArgument(json, type.tessellationShader, "tesselation_shader");
        ParseArgument(json, type.sampleRateShading, "sample_rate_shading");
        ParseArgument(json, type.dualSrcBlend, "dial_src_blend");
        ParseArgument(json, type.logicOp, "logic_op");
        ParseArgument(json, type.multiDrawIndirect, "multi_draw_indirect");
        ParseArgument(json, type.drawIndirectFirstInstance, "draw_indirect_first_instance");
        ParseArgument(json, type.depthClamp, "depth_clamp");
        ParseArgument(json, type.depthBiasClamp, "depth_bias_clamp");
        ParseArgument(json, type.fillModeNonSolid, "full_mode_non_solid");
        ParseArgument(json, type.depthBounds, "depth_bounds");
        ParseArgument(json, type.wideLines, "wide_lines");
        ParseArgument(json, type.largePoints, "large_points");
        ParseArgument(json, type.alphaToOne, "alpha_to_one");
        ParseArgument(json, type.multiViewport, "multi_viewport");
        ParseArgument(json, type.samplerAnisotropy, "sampler_anisotropy");
        ParseArgument(json, type.textureCompressionETC2, "texture_compression_etc2");
        ParseArgument(json, type.textureCompressionASTC_LDR, "texture_compression_astc_ldr");
        ParseArgument(json, type.textureCompressionBC, "texture_compression_bc");
        ParseArgument(json, type.occlusionQueryPrecise, "occlusion_query_precise");
        ParseArgument(json, type.pipelineStatisticsQuery, "pipeline_statistics_query");
        ParseArgument(json, type.vertexPipelineStoresAndAtomics, "vertex_pipeline_stores_and_atomics");
        ParseArgument(json, type.fragmentStoresAndAtomics, "fragment_stores_and_atomics");
        ParseArgument(json, type.shaderTessellationAndGeometryPointSize, "shader_tessellation_and_geometry_point_size");
        ParseArgument(json, type.shaderImageGatherExtended, "shader_image_gather_extended");
        ParseArgument(json, type.shaderStorageImageExtendedFormats, "shader_storage_image_extended_formats");
        ParseArgument(json, type.shaderStorageImageMultisample, "shader_storage_image_multisample");
        ParseArgument(json, type.shaderStorageImageReadWithoutFormat, "shader_storage_image_read_without_format");
        ParseArgument(json, type.shaderStorageImageWriteWithoutFormat, "shader_storage_image_write_without_format");
        ParseArgument(json, type.shaderUniformBufferArrayDynamicIndexing, "shader_uniform_buffer_array_dynamic_indexing");
        ParseArgument(json, type.shaderSampledImageArrayDynamicIndexing, "shader_sampled_image_array_dynamic_indexing");
        ParseArgument(json, type.shaderStorageBufferArrayDynamicIndexing, "shader_storage_buffer_array_dynamic_indexing");
        ParseArgument(json, type.shaderStorageImageArrayDynamicIndexing, "shader_storage_image_array_dynamic_indexing");
        ParseArgument(json, type.shaderClipDistance, "shader_clip_distance");
        ParseArgument(json, type.shaderCullDistance, "shader_cull_distance");
        ParseArgument(json, type.shaderFloat64, "shader_float64");
        ParseArgument(json, type.shaderInt64, "shader_int64");
        ParseArgument(json, type.shaderInt16, "shader_int16");
        ParseArgument(json, type.shaderResourceMinLod, "shader_resource_residency");
        ParseArgument(json, type.shaderResourceResidency, "shader_resource_min_log");
        ParseArgument(json, type.sparseBinding, "sparse_binding");
        ParseArgument(json, type.sparseResidencyBuffer, "sparse_residency_buffer");
        ParseArgument(json, type.sparseResidencyImage2D, "sparse_residency_image_2d");
        ParseArgument(json, type.sparseResidencyImage3D, "sparse_residency_image_3d");
        ParseArgument(json, type.sparseResidency2Samples, "sparse_residency_2_samples");
        ParseArgument(json, type.sparseResidency4Samples, "sparse_residency_4_samples");
        ParseArgument(json, type.sparseResidency8Samples, "sparse_residency_8_samples");
        ParseArgument(json, type.sparseResidency16Samples, "sparse_residency_16_samples");
        ParseArgument(json, type.sparseResidencyAliased, "sparse_residency_aliased");
        ParseArgument(json, type.variableMultisampleRate, "variable_multisample_rate");
        ParseArgument(json, type.fillModeNonSolid, "full_mode_non_soloid");
        ParseArgument(json, type.inheritedQueries, "inherited_queries");
    }
}

namespace engine
{
    namespace core
    {
        void to_json(nlohmann::json &json, const FDeviceCreateInfo &type)
        {
            std::vector<std::string> validationLayers{type.validationLayers.begin(), type.validationLayers.end()}, 
            deviceExtensions{type.deviceExtensions.begin(), type.deviceExtensions.end()};
            json = nlohmann::json{
                {"app_info", type.appInfo},
                {"validation", type.validation},
                {"validation_layers", validationLayers},
                {"device_extensions", deviceExtensions},
                {"physical_device_extensions", type.deviceFeatures}};
        }

        void from_json(const nlohmann::json &json, FDeviceCreateInfo &type)
        {
            std::vector<std::string> validationLayers, deviceExtensions;
            ParseArgument(json, type.appInfo, "app_info", true);
            ParseArgument(json, type.validation, "validation");
            ParseArgument(json, validationLayers, "validation_layers", true);
            ParseArgument(json, deviceExtensions, "device_extensions", true);
            ParseArgument(json, type.deviceFeatures, "device_features", true);

            ParseArgument(json, type.graphics, "graphics", true);

            type.validationLayers = str_vector_to_char_ptr_vector(validationLayers);
            type.deviceExtensions = str_vector_to_char_ptr_vector(deviceExtensions);
        }

        void to_json(nlohmann::json &json, const FDeviceGraphicsInfo &type)
        {
            json = nlohmann::json{
                {"multisampling", type.multisampling},
                {"post_process", type.postProcess},
                {"ray_tracing", type.rayTracing}};
        }

        void from_json(const nlohmann::json &json, FDeviceGraphicsInfo &type)
        {
            ParseArgument(json, type.multisampling, "multisampling", true);
            ParseArgument(json, type.postProcess, "post_process");
            ParseArgument(json, type.rayTracing, "ray_tracing");
        }
    }
}