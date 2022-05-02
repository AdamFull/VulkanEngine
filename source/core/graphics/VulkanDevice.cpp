#include "VulkanHighLevel.h"
#include "VulkanStaticHelper.h"

using namespace Engine::Core;
using namespace Engine::Core::Window;

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

CDevice::~CDevice()
{
    // surface is created by glfw, therefore not using a Unique handle
    destroy(m_surface);

    if (m_bValidation)
        destroyDebugUtilsMessengerEXT(m_vkInstance, m_vkDebugUtils, nullptr);

    vkDestroyDevice(m_logical, nullptr);
    vkDestroyInstance(m_vkInstance, nullptr);
}

// TODO: add features picking while initialization
void CDevice::create(const FDeviceCreateInfo& deviceCI)
{
    m_bValidation = deviceCI.validation;
    createInstance(deviceCI);
    createDebugCallback(deviceCI);
    createSurface();
    createDevice(deviceCI);
    m_pCommandPool = std::make_unique<CCommandPool>();
}

uint32_t CDevice::getVulkanVersion()
{
	if (m_vkInstance)
		return VK_API_VERSION_1_0;

	return 0;
}

void CDevice::createInstance(const FDeviceCreateInfo& deviceCI)
{
    if (deviceCI.validation && !VulkanStaticHelper::CheckValidationLayerSupport(deviceCI.validationLayers))
    {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    auto extensions = VulkanStaticHelper::GetRequiredExtensions(deviceCI.validation);

    auto createInfo = vk::InstanceCreateInfo{};
    createInfo.flags = vk::InstanceCreateFlags();
    createInfo.pApplicationInfo = &deviceCI.appInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();


    if (deviceCI.validation)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(deviceCI.validationLayers.size());
        createInfo.ppEnabledLayerNames = deviceCI.validationLayers.data();
    }

    m_vkInstance = vk::createInstance(createInfo, nullptr);
    assert(m_vkInstance && "Vulkan instance was not created.");
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
    if (createDebugUtilsMessengerEXT(m_vkInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT *>(&createInfo), nullptr, &m_vkDebugUtils) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug callback!");
    }
}

void CDevice::createSurface()
{
    assert(m_vkInstance && "Unable to create surface, cause vulkan instance is not valid");
    UWinHandle->createWindowSurface(m_vkInstance, m_surface);
    assert(m_surface && "Surface creation failed");
}

void CDevice::createDevice(const FDeviceCreateInfo& deviceCI)
{
    assert(m_vkInstance && "Unable to create ligical device, cause vulkan instance is not valid");
    m_physical = getPhysicalDevice(deviceCI.deviceExtensions);
    assert(m_physical && "No avaliable physical devices. Check device dependencies.");

    if(isSupportedSampleCount(deviceCI.graphics.multisampling))
        m_msaaSamples = deviceCI.graphics.multisampling;
    else
        m_msaaSamples = vk::SampleCountFlagBits::e1;

    QueueFamilyIndices indices = findQueueFamilies(m_physical);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        queueCreateInfos.push_back({vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority});
    }

    auto createInfo = vk::DeviceCreateInfo(
        vk::DeviceCreateFlags(),
        static_cast<uint32_t>(queueCreateInfos.size()),
        queueCreateInfos.data());

    createInfo.pEnabledFeatures = &deviceCI.deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceCI.deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceCI.deviceExtensions.data();

    if (deviceCI.validation)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(deviceCI.validationLayers.size());
        createInfo.ppEnabledLayerNames = deviceCI.validationLayers.data();
    }

    m_logical = m_physical.createDevice(createInfo);
    assert(m_logical && "Failed to create logical device.");

    m_qGraphicsQueue = m_logical.getQueue(indices.graphicsFamily.value(), 0);
    assert(m_qGraphicsQueue && "Failed while getting graphics queue.");
    m_qPresentQueue = m_logical.getQueue(indices.presentFamily.value(), 0);
    assert(m_qPresentQueue && "Failed while getting present queue.");
    m_qComputeQueue = m_logical.getQueue(indices.computeFamily.value(), 0);
    assert(m_qComputeQueue && "Failed while getting compute queue.");
    m_qTransferQueue = m_logical.getQueue(indices.transferFamily.value(), 0);
    assert(m_qTransferQueue && "Failed while getting transfer queue.");
}

uint32_t CDevice::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
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

QueueFamilyIndices CDevice::findQueueFamilies()
{
    return findQueueFamilies(m_physical);
}

SwapChainSupportDetails CDevice::querySwapChainSupport(const vk::PhysicalDevice &device)
{
    SwapChainSupportDetails details;
    details.capabilities = device.getSurfaceCapabilitiesKHR(m_surface);
    details.formats = device.getSurfaceFormatsKHR(m_surface);
    details.presentModes = device.getSurfacePresentModesKHR(m_surface);

    return details;
}

SwapChainSupportDetails CDevice::querySwapChainSupport()
{
    return querySwapChainSupport(m_physical);
}

std::vector<vk::SampleCountFlagBits> CDevice::getAvaliableSampleCount()
{
    vk::PhysicalDeviceProperties physicalDeviceProperties;
    m_physical.getProperties(&physicalDeviceProperties);
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

void CDevice::createOnDeviceBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory)
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
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    bufferMemory = m_logical.allocateMemory(allocInfo);
    assert(bufferMemory && "Buffer memory was not allocated");

    m_logical.bindBufferMemory(buffer, bufferMemory, 0);
}

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

vk::PhysicalDevice CDevice::getPhysicalDevice(const std::vector<const char*>& deviceExtensions)
{
    auto device = m_vkInstance.enumeratePhysicalDevices().front();
    if (device && isDeviceSuitable(device, deviceExtensions))
    {
        return device;
    }
    return nullptr;
}

std::vector<vk::PhysicalDevice> CDevice::getAvaliablePhysicalDevices(const std::vector<const char*>& deviceExtensions)
{
    auto devices = m_vkInstance.enumeratePhysicalDevices();
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

    bool extensionsSupported = VulkanStaticHelper::CheckDeviceExtensionSupport(device, deviceExtensions);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    vk::PhysicalDeviceFeatures supportedFeatures{};
    supportedFeatures = device.getFeatures();

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

namespace Engine
{
    namespace Core
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