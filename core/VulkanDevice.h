#pragma once
#include "WindowHandle.h"
#include "DataTypes/QueueFamilyIndicies.h"
#include "DataTypes/SwapChainSupportDetails.h"

namespace Engine
{
    #ifdef NDEBUG
        constexpr bool bEnableValidationLayers = false;
    #else
        constexpr bool bEnableValidationLayers = true;
    #endif

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    class VulkanDevice
    {
    public:
        VulkanDevice(std::unique_ptr<WindowHandle>& window);
        ~VulkanDevice();

        VulkanDevice(const VulkanDevice &) = delete;
        void operator=(const VulkanDevice &) = delete;
        VulkanDevice(VulkanDevice &&) = delete;
        VulkanDevice &operator=(VulkanDevice &&) = delete;

        SwapChainSupportDetails GetSwapChainSupport();
        uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
        QueueFamilyIndices FindPhysicalQueueFamilies();
        vk::Format FindSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
    
        void CreateOnDeviceBuffer(vk::DeviceSize size, vk::BufferUsageFlags buffer_usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory);
        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> 
        CreateCommandBuffer(vk::CommandBufferLevel level, vk::CommandPool& pool, uint32_t count);
        vk::CommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(vk::CommandBuffer& commandBuffer);
        void CopyOnDeviceBuffer(vk::Buffer& srcBuffer, vk::Buffer& dstBuffer, vk::DeviceSize size);
        void CopyBufferToImage(vk::Buffer& buffer, vk::Image& image, uint32_t width, uint32_t height, uint32_t layerCount);

        void CreateImage(vk::Image& image, vk::DeviceMemory& memory, uint32_t width, uint32_t height, uint32_t mip_levels, 
        vk::SampleCountFlagBits num_samples, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, 
        vk::MemoryPropertyFlags properties);

        vk::ImageView CreateImageView(vk::Image& pImage, uint32_t mip_levels, vk::Format eFormat, vk::ImageAspectFlags aspectFlags);
    
    private:
        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface(std::unique_ptr<WindowHandle>& window);
        vk::PhysicalDevice GetPhysicalDevice();
        void CreateLogicalDevice();
        void CreateCommandPool();

        // helper functions
        bool IsDeviceSuitable(vk::PhysicalDevice device);
        std::vector<const char *> GetRequiredExtensions();
        bool CheckValidationLayerSupport();
        QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);
        void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT &createInfo);
        bool CheckDeviceExtensionSupport(vk::PhysicalDevice device);
        SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device);
    private:
        vk::UniqueInstance m_pInstance;
        VkDebugUtilsMessengerEXT debugMessenger;
        vk::PhysicalDevice m_pPhysicalDevice{nullptr};
        vk::CommandPool m_CommandPool;

        vk::UniqueDevice m_pLogicalDevice;
        vk::SurfaceKHR m_Surface;
        vk::Queue m_qGraphicsQueue;
        vk::Queue m_qPresentQueue;
    };
    
}