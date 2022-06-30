#pragma once
#include "data_types/VulkanQueueFamilies.h"
#include "data_types/VulkanSwapChainSipportDetails.h"
#include "serializer/Serialization.hpp"
#include "graphics/commands/CommandBuffer.h"

namespace engine
{
    namespace core
    {
        struct FDeviceGraphicsInfo
        {
            vk::SampleCountFlagBits multisampling{};
            bool postProcess;
            bool rayTracing;
        };

        struct FDeviceCreateInfo
        {
            vk::ApplicationInfo appInfo;
            bool validation;
            std::vector<const char *> validationLayers;
            std::vector<const char *> deviceExtensions;
            vk::PhysicalDeviceFeatures deviceFeatures;

            FDeviceGraphicsInfo graphics;
        };

        class CDevice : public utl::singleton<CDevice>
        {
        public:
            CDevice() = default;

            static VkResult createDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT *, const VkAllocationCallbacks *, VkDebugUtilsMessengerEXT *);
            static VKAPI_ATTR VkBool32 VKAPI_CALL validationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT *, void *);
            static void destroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks *);

            void create(const FDeviceCreateInfo &deviceCI);
            void cleanup();
            void tryRebuildSwapchain();
            void updateCommandPools();

            vk::Result acquireNextImage(uint32_t *imageIndex);
            vk::Result submitCommandBuffers(const vk::CommandBuffer *commandBuffer, uint32_t *imageIndex, vk::QueueFlagBits queueBit);

            inline void GPUWait() { vkDevice.waitIdle(); }

            uint32_t getVulkanVersion();
            /***************************************************Helpers***************************************************/
            uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
            QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
            QueueFamilyIndices findQueueFamilies();
            SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice &device);
            SwapChainSupportDetails querySwapChainSupport();
            vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &surfaceCapabilities);
            std::vector<vk::SampleCountFlagBits> getAvaliableSampleCount();
            bool isSupportedSampleCount(vk::SampleCountFlagBits samples);

            /*************************************Data transfer***************************************************/
            template <class T>
            void moveToMemory(T raw_data, vk::DeviceMemory &memory, vk::DeviceSize size)
            {
                void *indata = vkDevice->mapMemory(memory, 0, size);
                memcpy(indata, raw_data, static_cast<size_t>(size));
                vkDevice->unmapMemory(memory);
            }

            /*****************************************Image work helpers*****************************************/
            void copyOnDeviceBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

            /**************************************************Getters********************************************/
            inline vk::Instance &getVkInstance() { return vkInstance; }
            inline vk::SurfaceKHR &getSurface() { return vkSurface; }
            const ref_ptr<CCommandPool> &getCommandPool(const std::thread::id &threadId = std::this_thread::get_id());

            inline vk::PhysicalDevice &getPhysical() { return vkPhysical; }
            inline vk::Device &getLogical() { return vkDevice; }
            inline vk::Queue &getGraphicsQueue() { return m_qGraphicsQueue; }
            inline vk::Queue &getPresentQueue() { return m_qPresentQueue; }
            inline vk::Queue &getComputeQueue() { return m_qComputeQueue; }
            inline vk::Queue &getTransferQueue() { return m_qTransferQueue; }

            inline vk::PipelineCache &getPipelineCache() { return pipelineCache; }

            inline vk::SampleCountFlagBits getSamples() { return m_msaaSamples; }
            inline vk::AllocationCallbacks *getAllocator() { return pAllocator; }

            // Swapchain getters
            inline vk::Format getImageFormat() { return imageFormat; };
            vk::Extent2D getExtent(bool automatic = false);
            inline std::vector<vk::Image> &getImages() { return vImages; }
            inline std::vector<vk::ImageView> &getImageViews() { return vImageViews; }
            inline vk::SwapchainKHR &GetSwapChain() { return swapChain; }
            inline std::vector<vk::Semaphore> &getImageAvailableSemaphores() { return vImageAvailableSemaphores; }
            inline std::vector<vk::Semaphore> &getRenderFinishedSemaphores() { return vRenderFinishedSemaphores; }
            inline std::vector<vk::Fence> &getInFlightFences() { return vInFlightFences; }
            inline size_t getCurrentFrame() { return currentFrame; }
            inline uint32_t getFramesInFlight() { return framesInFlight; }
            float getAspect(bool automatic = false);
            inline bool getReduildFlag() { return bSwapChainRebuild; }
            inline void setRebuildFlag() { bSwapChainRebuild = true; }

            void setViewportExtent(vk::Extent2D extent);
            inline const bool isNeedToRebuildViewport() const { return bViewportRebuild; }

            template <class _Ty, class _Ret>
            vk::Result create(_Ty &info, _Ret *ref)
            {
                assert(false && "Unknown object type.");
                return vk::Result::eSuccess;
            }
            template <>
            vk::Result create(vk::InstanceCreateInfo &info, vk::Instance *ref)
            {
                return vk::createInstance(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::DeviceCreateInfo &info, vk::Device *ref)
            {
                return vkPhysical.createDevice(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::SwapchainCreateInfoKHR &info, vk::SwapchainKHR *ref)
            {
                return vkDevice.createSwapchainKHR(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::RenderPassCreateInfo &info, vk::RenderPass *ref)
            {
                return vkDevice.createRenderPass(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::FramebufferCreateInfo &info, vk::Framebuffer *ref)
            {
                return vkDevice.createFramebuffer(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::SemaphoreCreateInfo &info, vk::Semaphore *ref)
            {
                return vkDevice.createSemaphore(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::FenceCreateInfo &info, vk::Fence *ref)
            {
                return vkDevice.createFence(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::ShaderModuleCreateInfo &info, vk::ShaderModule *ref)
            {
                return vkDevice.createShaderModule(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::PipelineLayoutCreateInfo &info, vk::PipelineLayout *ref)
            {
                return vkDevice.createPipelineLayout(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::GraphicsPipelineCreateInfo &info, vk::Pipeline *ref)
            {
                return vkDevice.createGraphicsPipelines(pipelineCache, 1, &info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::ComputePipelineCreateInfo &info, vk::Pipeline *ref)
            {
                return vkDevice.createComputePipelines(pipelineCache, 1, &info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::DescriptorSetLayoutCreateInfo &info, vk::DescriptorSetLayout *ref)
            {
                return vkDevice.createDescriptorSetLayout(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::MemoryAllocateInfo &info, vk::DeviceMemory *ref)
            {
                return vkDevice.allocateMemory(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::ImageCreateInfo &info, vk::Image *ref)
            {
                return vkDevice.createImage(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::ImageViewCreateInfo &info, vk::ImageView *ref)
            {
                return vkDevice.createImageView(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::SamplerCreateInfo &info, vk::Sampler *ref)
            {
                return vkDevice.createSampler(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::CommandPoolCreateInfo &info, vk::CommandPool *ref)
            {
                return vkDevice.createCommandPool(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::CommandBufferAllocateInfo &info, vk::CommandBuffer *ref)
            {
                return vkDevice.allocateCommandBuffers(&info, ref);
            }
            template <>
            vk::Result create(vk::DescriptorSetAllocateInfo &info, vk::DescriptorSet *ref)
            {
                return vkDevice.allocateDescriptorSets(&info, ref);
            }
            template <>
            vk::Result create(vk::PipelineCacheCreateInfo &info, vk::PipelineCache *ref)
            {
                return vkDevice.createPipelineCache(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::BufferCreateInfo &info, vk::Buffer *ref)
            {
                return vkDevice.createBuffer(&info, pAllocator, ref);
            }
            template <>
            vk::Result create(vk::DescriptorPoolCreateInfo &info, vk::DescriptorPool *ref)
            {
                return vkDevice.createDescriptorPool(&info, pAllocator, ref);
            }

            template <class _Ty>
            void destroy(_Ty *ref) { assert(false && "Unknown object type."); }
            template <>
            void destroy(vk::Instance *ref) { vkDestroyInstance(*ref, (const VkAllocationCallbacks *)pAllocator); }
            template <>
            void destroy(vk::Device *ref) { vkDestroyDevice(*ref, (const VkAllocationCallbacks *)pAllocator); }
            template <>
            void destroy(vk::Sampler *ref) { vkDevice.destroySampler(*ref, pAllocator); }
            template <>
            void destroy(vk::Image *ref) { vkDevice.destroyImage(*ref, pAllocator); }
            template <>
            void destroy(vk::ImageView *ref) { vkDevice.destroyImageView(*ref, pAllocator); }
            template <>
            void destroy(vk::Buffer *ref) { vkDevice.destroyBuffer(*ref, pAllocator); }
            template <>
            void destroy(vk::DeviceMemory *ref) { vkDevice.freeMemory(*ref, pAllocator); }
            template <>
            void destroy(vk::Framebuffer *ref) { vkDevice.destroyFramebuffer(*ref, pAllocator); }
            template <>
            void destroy(vk::RenderPass *ref) { vkDevice.destroyRenderPass(*ref, pAllocator); }
            template <>
            void destroy(vk::SwapchainKHR *ref) { vkDevice.destroySwapchainKHR(*ref, pAllocator); }
            template <>
            void destroy(vk::Pipeline *ref) { vkDevice.destroyPipeline(*ref, pAllocator); }
            template <>
            void destroy(vk::PipelineLayout *ref) { vkDevice.destroyPipelineLayout(*ref, pAllocator); }
            template <>
            void destroy(vk::PipelineCache *ref) { vkDevice.destroyPipelineCache(*ref, pAllocator); }
            template <>
            void destroy(vk::ShaderModule *ref) { vkDevice.destroyShaderModule(*ref, pAllocator); }
            template <>
            void destroy(vk::CommandPool *ref) { vkDevice.destroyCommandPool(*ref, pAllocator); }
            template <>
            void destroy(vk::SurfaceKHR *ref) { vkInstance.destroySurfaceKHR(*ref, pAllocator); }
            template <>
            void destroy(vk::DescriptorSetLayout *ref) { vkDevice.destroyDescriptorSetLayout(*ref, pAllocator); }
            template <>
            void destroy(vk::DescriptorPool *ref) { vkDevice.destroyDescriptorPool(*ref, pAllocator); }
            template <>
            void destroy(vk::Semaphore *ref) { vkDevice.destroySemaphore(*ref, pAllocator); }
            template <>
            void destroy(vk::Fence *ref) { vkDevice.destroyFence(*ref, pAllocator); }

        private:
            void createInstance(const FDeviceCreateInfo &deviceCI);
            void createDebugCallback(const FDeviceCreateInfo &deviceCI);
            void createSurface();
            void createDevice(const FDeviceCreateInfo &deviceCI);
            void createPipelineCache();
            void createSwapchain();
            void cleanupSwapchain();

            // Private helpers
            vk::PhysicalDevice getPhysicalDevice(const std::vector<const char *> &deviceExtensions);
            std::vector<vk::PhysicalDevice> getAvaliablePhysicalDevices(const std::vector<const char *> &deviceExtensions);
            bool isDeviceSuitable(const vk::PhysicalDevice &device, const std::vector<const char *> &deviceExtensions);

        private:
            vk::Instance vkInstance{VK_NULL_HANDLE}; // Main vulkan instance
            VkDebugUtilsMessengerEXT m_vkDebugUtils{NULL};
            vk::SurfaceKHR vkSurface{VK_NULL_HANDLE}; // Vulkan's drawing surface
            std::map<std::thread::id, ref_ptr<CCommandPool>> commandPools;
            vk::AllocationCallbacks *pAllocator{nullptr};

            vk::PhysicalDevice vkPhysical;
            vk::Device vkDevice;
            vk::Queue m_qGraphicsQueue;
            vk::Queue m_qPresentQueue;
            vk::Queue m_qComputeQueue;
            vk::Queue m_qTransferQueue;

            vk::PipelineCache pipelineCache{VK_NULL_HANDLE};
            vk::SampleCountFlagBits m_msaaSamples{vk::SampleCountFlagBits::e1};
            bool m_bValidation{false};

            // Swapchain instances
            vk::Format imageFormat;
            vk::Extent2D swapchainExtent;
            vk::SwapchainKHR swapChain{VK_NULL_HANDLE};
            size_t currentFrame{0};
            uint32_t framesInFlight{4};
            bool bSwapChainRebuild{};

            vk::Extent2D viewportExtent;
            bool bViewportRebuild{false};

            std::vector<vk::Image> vImages;
            std::vector<vk::ImageView> vImageViews;
            std::vector<vk::Semaphore> vImageAvailableSemaphores;
            std::vector<vk::Semaphore> vRenderFinishedSemaphores;
            std::vector<vk::Fence> vInFlightFences;
        };

        REGISTER_SERIALIZATION_BLOCK_H(vk::ApplicationInfo);
        REGISTER_SERIALIZATION_BLOCK_H(vk::PhysicalDeviceFeatures);
        REGISTER_SERIALIZATION_BLOCK_H(FDeviceCreateInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FDeviceGraphicsInfo);
    }
}