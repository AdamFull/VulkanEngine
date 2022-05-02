#pragma once
#include "data_types/VulkanQueueFamilies.h"
#include "data_types/VulkanSwapChainSipportDetails.h"
#include "serializer/Serialization.hpp"
#include "graphics/commands/CommandBuffer.h"

namespace Engine
{
    namespace Core
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
            std::vector<const char*> validationLayers;
            std::vector<const char*> deviceExtensions;
            vk::PhysicalDeviceFeatures deviceFeatures;

            FDeviceGraphicsInfo graphics;
        };

        class Device
        {
        public:
            Device() = default;
            ~Device();

            Device(const Device &) = delete;
            void operator=(const Device &) = delete;
            Device(Device &&) = delete;
            Device &operator=(Device &&) = delete;

            static VkResult CreateDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT *, const VkAllocationCallbacks *, VkDebugUtilsMessengerEXT *);
            static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT *, void *);
            static void DestroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks *);

            void Create(const FDeviceCreateInfo& deviceCI);

            inline void GPUWait() { m_logical.waitIdle(); }

            uint32_t GetVulkanVersion();
            /***************************************************Helpers***************************************************/
            uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
            QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);
            QueueFamilyIndices FindQueueFamilies();
            SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice &device);
            SwapChainSupportDetails QuerySwapChainSupport();
            std::vector<vk::SampleCountFlagBits> GetAvaliableSampleCount();
            bool IsSupportedSampleCount(vk::SampleCountFlagBits samples);

            /*************************************Data transfer***************************************************/
            template <class T>
            void MoveToMemory(T raw_data, vk::DeviceMemory &memory, vk::DeviceSize size)
            {
                void *indata = m_logical->mapMemory(memory, 0, size);
                memcpy(indata, raw_data, static_cast<size_t>(size));
                m_logical->unmapMemory(memory);
            }

            /*****************************************Image work helpers*****************************************/

            void CreateOnDeviceBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory);
            void CopyOnDeviceBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

            /**************************************************Getters********************************************/
            inline vk::Instance &GetVkInstance() { return m_vkInstance; }
            inline vk::SurfaceKHR &GetSurface() { return m_surface; }
            inline std::shared_ptr<CCommandPool> GetCommandPool() { return m_pCommandPool; }

            inline vk::PhysicalDevice GetPhysical() { return m_physical; }
            inline vk::Device &GetLogical() { return m_logical; }
            inline vk::Queue &GetGraphicsQueue() { return m_qGraphicsQueue; }
            inline vk::Queue &GetPresentQueue() { return m_qPresentQueue; }
            inline vk::Queue &GetComputeQueue() { return m_qComputeQueue; }
            inline vk::Queue &GetTransferQueue() { return m_qTransferQueue; }

            inline vk::SampleCountFlagBits GetSamples() { return m_msaaSamples; }

            /********************************************Specializations***************************************/
            template <class OutType, class InitializerType>
            OutType Make(InitializerType initializer) 
            { 
                assert(false && "Device: no Make specialisation found for given type");
            }

            template <>
            vk::SwapchainKHR Make(vk::SwapchainCreateInfoKHR initializer) 
            { 
                return m_logical.createSwapchainKHR(initializer); 
            }

            template <>
            std::vector<vk::Image> Make(vk::SwapchainKHR initializer) 
            { 
                return m_logical.getSwapchainImagesKHR(initializer); 
            }

            template <>
            vk::RenderPass Make(vk::RenderPassCreateInfo initializer) 
            { 
                return m_logical.createRenderPass(initializer); 
            }

            template <>
            vk::Framebuffer Make(vk::FramebufferCreateInfo initializer) 
            { 
                return m_logical.createFramebuffer(initializer); 
            }

            template <>
            vk::Semaphore Make(vk::SemaphoreCreateInfo initializer) 
            { 
                return m_logical.createSemaphore(initializer); 
            }

            template <>
            vk::Fence Make(vk::FenceCreateInfo initializer) 
            { 
                return m_logical.createFence(initializer); 
            }

            template <>
            vk::ShaderModule Make(vk::ShaderModuleCreateInfo initializer) 
            { 
                return m_logical.createShaderModule(initializer);
            }

            template <>
            vk::PipelineLayout Make(vk::PipelineLayoutCreateInfo initializer) 
            { 
                return m_logical.createPipelineLayout(initializer);
            }

            template <>
            vk::Pipeline Make(vk::GraphicsPipelineCreateInfo initializer) 
            { 
                return m_logical.createGraphicsPipeline(nullptr, initializer).value; 
            }

            template <>
            vk::DescriptorPool Make(vk::DescriptorPoolCreateInfo initializer) 
            { 
                return m_logical.createDescriptorPool(initializer); 
            }

            //Destroyers
            template <class ToDestroy>
            void Destroy(ToDestroy &instance) 
            { 
                assert(false && "Device: no Destroy specialisation found for given type"); 
            } // Instance destroyer)

            template <>
            void Destroy(vk::Sampler &instance) 
            { 
                m_logical.destroySampler(instance); 
            }

            template <>
            void Destroy(vk::ImageView &instance) 
            { 
                m_logical.destroyImageView(instance); 
            }

            template <>
            void Destroy(vk::Image &instance) 
            { 
                m_logical.destroyImage(instance); 
            }

            template <>
            void Destroy(vk::Buffer &instance) 
            { 
                m_logical.destroyBuffer(instance); 
            }

            template <>
            void Destroy(vk::DeviceMemory &instance) 
            { 
                m_logical.freeMemory(instance); 
            }

            template <>
            void Destroy(vk::Framebuffer &instance) 
            { 
                m_logical.destroyFramebuffer(instance); 
            }

            template <>
            void Destroy(vk::RenderPass &instance) 
            { 
                m_logical.destroyRenderPass(instance); 
            }

            template <>
            void Destroy(vk::SwapchainKHR &instance) 
            { 
                m_logical.destroySwapchainKHR(instance); 
            }

            template <>
            void Destroy(vk::Pipeline &instance) 
            { 
                m_logical.destroyPipeline(instance); 
            }

            template <>
            void Destroy(vk::PipelineLayout &instance) 
            { 
                m_logical.destroyPipelineLayout(instance); 
            }

            template <>
            void Destroy(vk::PipelineCache &instance) 
            { 
                m_logical.destroyPipelineCache(instance); 
            }

            template <>
            void Destroy(vk::ShaderModule &instance) 
            {
                m_logical.destroyShaderModule(instance); 
            }

            template <>
            void Destroy(vk::CommandPool &instance) 
            { 
                m_logical.destroyCommandPool(instance); 
            }

            template <>
            void Destroy(vk::SurfaceKHR &instance) 
            { 
                m_vkInstance.destroySurfaceKHR(instance); 
            }

            template <>
            void Destroy(vk::DescriptorSetLayout &instance) 
            { 
                m_logical.destroyDescriptorSetLayout(instance); 
            }

            template <>
            void Destroy(vk::DescriptorPool &instance) 
            { 
                m_logical.destroyDescriptorPool(instance); 
            }

            template <>
            void Destroy(vk::Semaphore &instance) 
            { 
                m_logical.destroySemaphore(instance); 
            }

            template <>
            void Destroy(vk::Fence &instance) 
            { 
                m_logical.destroyFence(instance); 
            }

        private:
            void CreateInstance(const FDeviceCreateInfo& deviceCI);
            void CreateDebugCallback(const FDeviceCreateInfo& deviceCI);
            void CreateSurface();
            void CreateDevice(const FDeviceCreateInfo& deviceCI);

            // Private helpers
            vk::PhysicalDevice GetPhysicalDevice(const std::vector<const char*>& deviceExtensions);
            std::vector<vk::PhysicalDevice> GetAvaliablePhysicalDevices(const std::vector<const char*>& deviceExtensions);
            bool IsDeviceSuitable(const vk::PhysicalDevice &device, const std::vector<const char*>& deviceExtensions);

        private:
            vk::Instance m_vkInstance; // Main vulkan instance
            VkDebugUtilsMessengerEXT m_vkDebugUtils;
            vk::SurfaceKHR m_surface; // Vulkan's drawing surface
            std::shared_ptr<CCommandPool> m_pCommandPool{};

            vk::PhysicalDevice m_physical;
            vk::Device m_logical;
            vk::Queue m_qGraphicsQueue;
            vk::Queue m_qPresentQueue;
            vk::Queue m_qComputeQueue;
            vk::Queue m_qTransferQueue;

            vk::SampleCountFlagBits m_msaaSamples{vk::SampleCountFlagBits::e1};
            bool m_bValidation{false};
        };

        REGISTER_SERIALIZATION_BLOCK_H(vk::ApplicationInfo);
        REGISTER_SERIALIZATION_BLOCK_H(vk::PhysicalDeviceFeatures);
        REGISTER_SERIALIZATION_BLOCK_H(FDeviceCreateInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FDeviceGraphicsInfo);
    }
}