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

    SwapChainSupportDetails QuerySwapChainSupport(Main::FVulkanRenderInstance& renderInstance);
    uint32_t FindMemoryType(Main::FVulkanRenderInstance& renderInstance, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    QueueFamilyIndices FindQueueFamilies(Main::FVulkanRenderInstance& renderInstance);
    vk::Format FindSupportedFormat(Main::FVulkanRenderInstance& renderInstance, const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

    void CreateOnDeviceBuffer(Main::FVulkanRenderInstance& renderInstance, vk::DeviceSize size, vk::BufferUsageFlags buffer_usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory);
    std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>>
    CreateCommandBuffer(Main::FVulkanRenderInstance& renderInstance, vk::CommandBufferLevel level, vk::CommandPool &pool, uint32_t count);
    vk::CommandBuffer BeginSingleTimeCommands(Main::FVulkanRenderInstance& renderInstance);
    void EndSingleTimeCommands(Main::FVulkanRenderInstance& renderInstance, vk::CommandBuffer &commandBuffer);
    void CopyOnDeviceBuffer(Main::FVulkanRenderInstance& renderInstance, vk::Buffer &srcBuffer, vk::Buffer &dstBuffer, vk::DeviceSize size);

    //Image stuff
    void CreateImage(Main::FVulkanRenderInstance& renderInstance, vk::Image &image, vk::DeviceMemory &memory, uint32_t width, uint32_t height, uint32_t mip_levels,
                     vk::SampleCountFlagBits num_samples, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                     vk::MemoryPropertyFlags properties);
    
    void TransitionImageLayout(Main::FVulkanRenderInstance& renderInstance, vk::Image& image, uint32_t mip_levels, vk::ImageAspectFlags aspectFlags, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    void CopyBufferToImage(Main::FVulkanRenderInstance& renderInstance, vk::Buffer &buffer, vk::Image &image, uint32_t width, uint32_t height, uint32_t layerCount);
    vk::ImageView CreateImageView(Main::FVulkanRenderInstance& renderInstance, vk::Image &pImage, uint32_t mip_levels, vk::Format eFormat, vk::ImageAspectFlags aspectFlags);
    void GenerateMipmaps(Main::FVulkanRenderInstance& renderInstance, vk::Image &image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags);
    void CreateSampler(Main::FVulkanRenderInstance& renderInstance, vk::Sampler& sampler, uint32_t mip_levels);

    // helper functions
    bool IsDeviceSuitable(Main::FVulkanRenderInstance& renderInstance);
    std::vector<const char *> GetRequiredExtensions();
    bool CheckValidationLayerSupport();
    void PopulateDebugMessengerCreateInfo(Main::FVulkanRenderInstance& renderInstance, vk::DebugUtilsMessengerCreateInfoEXT &createInfo);
    bool CheckDeviceExtensionSupport(Main::FVulkanRenderInstance& renderInstance);
}