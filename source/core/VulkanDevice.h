#pragma once
#include "DataTypes/QueueFamilyIndicies.h"
#include "DataTypes/SwapChainSupportDetails.h"
#include "VulkanMainStructures.h"

namespace Engine::Device
{
#ifdef NDEBUG
    constexpr bool bEnableValidationLayers = false;
#else
    constexpr bool bEnableValidationLayers = true;
#endif

    const std::vector<const char *> vValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> vDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    vk::PhysicalDevice GetPhysicalDevice(Main::FVulkanEngine& engine);
    std::vector<vk::PhysicalDevice> GetAvaliablePhysicalDevices(Main::FVulkanEngine& engine);

    SwapChainSupportDetails QuerySwapChainSupport(Main::FVulkanEngine& engine);
    uint32_t FindMemoryType(Main::FVulkanEngine& engine, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    QueueFamilyIndices FindQueueFamilies(Main::FVulkanEngine& engine);
    vk::Format FindSupportedFormat(Main::FVulkanEngine& engine, const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

    void CreateOnDeviceBuffer(Main::FVulkanEngine& engine, vk::DeviceSize size, vk::BufferUsageFlags buffer_usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory);
    std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>>
    CreateCommandBuffer(Main::FVulkanEngine& engine, vk::CommandBufferLevel level, vk::CommandPool &pool, uint32_t count);
    vk::CommandBuffer BeginSingleTimeCommands(Main::FVulkanEngine& engine);
    void EndSingleTimeCommands(Main::FVulkanEngine& engine, vk::CommandBuffer &commandBuffer);
    void CopyOnDeviceBuffer(Main::FVulkanEngine& engine, vk::Buffer &srcBuffer, vk::Buffer &dstBuffer, vk::DeviceSize size);

    //Image stuff
    void CreateImage(Main::FVulkanEngine& engine, vk::Image &image, vk::DeviceMemory &memory, uint32_t width, uint32_t height, uint32_t mip_levels,
                     vk::SampleCountFlagBits num_samples, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                     vk::MemoryPropertyFlags properties);
    
    void TransitionImageLayout(Main::FVulkanEngine& engine, vk::Image& image, uint32_t mip_levels, vk::ImageAspectFlags aspectFlags, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    void CopyBufferToImage(Main::FVulkanEngine& engine, vk::Buffer &buffer, vk::Image &image, uint32_t width, uint32_t height, uint32_t layerCount);
    vk::ImageView CreateImageView(Main::FVulkanEngine& engine, vk::Image &pImage, uint32_t mip_levels, vk::Format eFormat, vk::ImageAspectFlags aspectFlags);
    void GenerateMipmaps(Main::FVulkanEngine& engine, vk::Image &image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags);
    void CreateSampler(Main::FVulkanEngine& engine, vk::Sampler& sampler, uint32_t mip_levels);

    vk::SampleCountFlagBits GetMaxUsableSampleCount(Main::FVulkanEngine& engine);

    // helper functions
    bool IsDeviceSuitable(Main::FVulkanEngine& engine);
    std::vector<const char *> GetRequiredExtensions();
    bool CheckValidationLayerSupport();
    void PopulateDebugMessengerCreateInfo(Main::FVulkanEngine& engine, vk::DebugUtilsMessengerCreateInfoEXT &createInfo);
    bool CheckDeviceExtensionSupport(Main::FVulkanEngine& engine);
}