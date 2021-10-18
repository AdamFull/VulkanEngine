#pragma once
#include "DataTypes/VulkanQueueFamilies.h"
#include "DataTypes/VulkanSwapChainSipportDetails.h"

namespace Engine
{
    class WindowHandle;

    struct FDevice
    {
        vk::UniqueInstance vkInstance;                       //Main vulkan instance
        VkDebugUtilsMessengerEXT vkDebugUtils;
        vk::SurfaceKHR surface;                              //Vulkan's drawing surface
        vk::CommandPool commandPool;

        vk::PhysicalDevice physical;
        vk::UniqueDevice logical;
        vk::Queue qGraphicsQueue;
        vk::Queue qPresentQueue;

        vk::SampleCountFlagBits msaaSamples = vk::SampleCountFlagBits::e1;
    };

    VkResult CreateDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);
    VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);
    void DestroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);

    class Device
    {
    public:
        Device() = default;
        Device(const Device&) = delete;
        void operator=(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(Device&&) = delete;

        void Create(std::unique_ptr<WindowHandle>& pWindow, const char *pApplicationName, uint32_t applicationVersion,
                                         const char *pEngineName, uint32_t engineVersion,
                                         uint32_t apiVersion);

        inline void GPUWait() { data.logical->waitIdle(); }

        void Cleanup();

        /***************************************************Helpers***************************************************/
        vk::Format FindSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
        uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
        QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);
        QueueFamilyIndices FindQueueFamilies();
        SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice &device);
        SwapChainSupportDetails QuerySwapChainSupport();
        vk::SampleCountFlagBits GetMaxUsableSampleCount();

        /*************************************Data transfer***************************************************/
        template<class T>
        void MoveToMemory(T raw_data, vk::DeviceMemory& memory, vk::DeviceSize size)
        {
            void *indata = data.logical->mapMemory(memory, 0, size);
            memcpy(indata, raw_data, static_cast<size_t>(size));
            data.logical->unmapMemory(memory);
        }

        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> 
        CreateCommandBuffer(vk::CommandBufferLevel level, vk::CommandPool& pool, uint32_t count);
        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> 
        CreateCommandBuffer(vk::CommandBufferLevel level, uint32_t count);
        vk::CommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);

        /*****************************************Image work helpers*****************************************/
        void CreateImage(vk::Image& image, vk::DeviceMemory& memory, vk::ImageCreateInfo createInfo, vk::MemoryPropertyFlags properties);
        void TransitionImageLayout(vk::Image& image, std::vector<vk::ImageMemoryBarrier> vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
        void CopyBufferToImage(vk::Buffer& buffer, vk::Image& image, std::vector<vk::BufferImageCopy> vRegions);
        vk::ImageView CreateImageView(vk::Image& pImage, vk::ImageViewCreateInfo viewInfo);
        void CreateSampler(vk::Sampler& sampler, uint32_t mip_levels, vk::SamplerAddressMode eAddressMode);

        void CreateOnDeviceBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory);
        void CopyOnDeviceBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

        /**************************************************Getters********************************************/
        inline vk::UniqueInstance& GetVkInstance() { return data.vkInstance; }
        inline vk::SurfaceKHR& GetSurface() { return data.surface; }
        inline vk::CommandPool& GetCommandPool() { return data.commandPool; }

        inline vk::PhysicalDevice GetPhysical() { return data.physical; }
        inline vk::UniqueDevice& GetLogical() { return data.logical; }
        inline vk::Queue& GetGraphicsQueue() { return data.qGraphicsQueue; }
        inline vk::Queue& GetPresentQueue() { return data.qPresentQueue; }

        inline vk::SampleCountFlagBits GetSamples() { return data.msaaSamples; }

        /********************************************Specializations***************************************/
        template<class OutType, class InitializerType> OutType Make(InitializerType& initializer) { assert(false && "Device: no Make specialisation found for given type"); }
        template<> vk::SwapchainKHR Make(vk::SwapchainCreateInfoKHR& initializer) { return data.logical->createSwapchainKHR(initializer); }
        template<> std::vector<vk::Image> Make(vk::SwapchainKHR& initializer) { return data.logical->getSwapchainImagesKHR(initializer); }
        template<> vk::RenderPass Make(vk::RenderPassCreateInfo& initializer) { return data.logical->createRenderPass(initializer); }
        template<> vk::Framebuffer Make(vk::FramebufferCreateInfo& initializer) { return data.logical->createFramebuffer(initializer); }
        template<> vk::Semaphore Make(vk::SemaphoreCreateInfo& initializer) { return data.logical->createSemaphore(initializer); }
        template<> vk::Fence Make(vk::FenceCreateInfo& initializer) { return data.logical->createFence(initializer); }
        template<> vk::ShaderModule Make(vk::ShaderModuleCreateInfo& initializer) { return data.logical->createShaderModule(initializer); }
        template<> vk::PipelineLayout Make(vk::PipelineLayoutCreateInfo& initializer) { return data.logical->createPipelineLayout(initializer); }
        template<> vk::Pipeline Make(vk::GraphicsPipelineCreateInfo& initializer) { return data.logical->createGraphicsPipeline(nullptr, initializer).value; }
        template<> vk::DescriptorPool Make(vk::DescriptorPoolCreateInfo& initializer) { return data.logical->createDescriptorPool(initializer); }

        template<class ToDestroy> void Destroy(ToDestroy& instance) { assert(false && "Device: no Destroy specialisation found for given type"); }                                              //Instance destroyer)
        template<> void Destroy(vk::Sampler& instance) { data.logical->destroySampler(instance); }
        template<> void Destroy(vk::ImageView& instance) { data.logical->destroyImageView(instance); }
        template<> void Destroy(vk::Image& instance) { data.logical->destroyImage(instance); }
        template<> void Destroy(vk::Buffer& instance) { data.logical->destroyBuffer(instance); }
        template<> void Destroy(vk::DeviceMemory& instance) { data.logical->freeMemory(instance); }
        template<> void Destroy(vk::Framebuffer& instance) { data.logical->destroyFramebuffer(instance); }
        template<> void Destroy(vk::RenderPass& instance) { data.logical->destroyRenderPass(instance); }
        template<> void Destroy(vk::SwapchainKHR& instance) { data.logical->destroySwapchainKHR(instance); }
        template<> void Destroy(vk::Pipeline& instance) { data.logical->destroyPipeline(instance); }
        template<> void Destroy(vk::PipelineLayout& instance) { data.logical->destroyPipelineLayout(instance); }
        template<> void Destroy(vk::PipelineCache& instance) { data.logical->destroyPipelineCache(instance); }
        template<> void Destroy(vk::ShaderModule& instance) { data.logical->destroyShaderModule(instance); }
        template<> void Destroy(std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>>& instance) { data.logical->freeCommandBuffers(data.commandPool, instance); }
        template<> void Destroy(vk::CommandPool& instance) { data.logical->destroyCommandPool(instance); }
        template<> void Destroy(vk::SurfaceKHR& instance) { data.vkInstance->destroySurfaceKHR(instance); }
        template<> void Destroy(vk::DescriptorSetLayout& instance) { data.logical->destroyDescriptorSetLayout(instance); }
        template<> void Destroy(vk::DescriptorPool& instance) { data.logical->destroyDescriptorPool(instance); }
        template<> void Destroy(vk::Semaphore& instance) { data.logical->destroySemaphore(instance); }
        template<> void Destroy(vk::Fence& instance) { data.logical->destroyFence(instance); }
    private:
        void CreateInstance(const char *pApplicationName, uint32_t applicationVersion,
                                         const char *pEngineName, uint32_t engineVersion,
                                         uint32_t apiVersion);
        void CreateDebugCallback();
        void CreateSurface(std::unique_ptr<WindowHandle>& pWindow);
        void CreateDevice();
        void CreateCommandPool();

        //Private helpers
        vk::PhysicalDevice GetPhysicalDevice();
        std::vector<vk::PhysicalDevice> GetAvaliablePhysicalDevices();
        bool IsDeviceSuitable(const vk::PhysicalDevice &device);

        FDevice data;
    };
}