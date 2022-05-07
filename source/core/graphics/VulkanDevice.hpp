#pragma once
#include "data_types/VulkanQueueFamilies.h"
#include "data_types/VulkanSwapChainSipportDetails.h"
#include "serializer/Serialization.hpp"
#include "graphics/commands/CommandBuffer.h"
#include <util/helpers.hpp>

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

        class CDevice : public utl::singleton<CDevice>
        {
        public:
            CDevice() = default;

            static VkResult createDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT *, const VkAllocationCallbacks *, VkDebugUtilsMessengerEXT *);
            static VKAPI_ATTR VkBool32 VKAPI_CALL validationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT *, void *);
            static void destroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks *);

            void create(const FDeviceCreateInfo& deviceCI);
            void cleanup();

            inline void GPUWait() { m_logical.waitIdle(); }

            uint32_t getVulkanVersion();
            /***************************************************Helpers***************************************************/
            uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
            QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
            QueueFamilyIndices findQueueFamilies();
            SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice &device);
            SwapChainSupportDetails querySwapChainSupport();
            std::vector<vk::SampleCountFlagBits> getAvaliableSampleCount();
            bool isSupportedSampleCount(vk::SampleCountFlagBits samples);

            /*************************************Data transfer***************************************************/
            template <class T>
            void moveToMemory(T raw_data, vk::DeviceMemory &memory, vk::DeviceSize size)
            {
                void *indata = m_logical->mapMemory(memory, 0, size);
                memcpy(indata, raw_data, static_cast<size_t>(size));
                m_logical->unmapMemory(memory);
            }

            /*****************************************Image work helpers*****************************************/

            void createOnDeviceBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory);
            void copyOnDeviceBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

            /**************************************************Getters********************************************/
            inline vk::Instance &getVkInstance() { return m_vkInstance; }
            inline vk::SurfaceKHR &getSurface() { return m_surface; }
            inline std::shared_ptr<CCommandPool> getCommandPool() { return m_pCommandPool; }

            inline vk::PhysicalDevice getPhysical() { return m_physical; }
            inline vk::Device &getLogical() { return m_logical; }
            inline vk::Queue &getGraphicsQueue() { return m_qGraphicsQueue; }
            inline vk::Queue &getPresentQueue() { return m_qPresentQueue; }
            inline vk::Queue &getComputeQueue() { return m_qComputeQueue; }
            inline vk::Queue &getTransferQueue() { return m_qTransferQueue; }

            inline vk::SampleCountFlagBits getSamples() { return m_msaaSamples; }

            /********************************************Specializations***************************************/
            template <class OutType, class InitializerType>
            OutType make(InitializerType initializer) 
            { 
                assert(false && "Device: no Make specialisation found for given type");
            }

            template <>
            vk::SwapchainKHR make(vk::SwapchainCreateInfoKHR initializer) 
            { 
                return m_logical.createSwapchainKHR(initializer); 
            }

            template <>
            std::vector<vk::Image> make(vk::SwapchainKHR initializer) 
            { 
                return m_logical.getSwapchainImagesKHR(initializer); 
            }

            template <>
            vk::RenderPass make(vk::RenderPassCreateInfo initializer) 
            { 
                return m_logical.createRenderPass(initializer); 
            }

            template <>
            vk::Framebuffer make(vk::FramebufferCreateInfo initializer) 
            { 
                return m_logical.createFramebuffer(initializer); 
            }

            template <>
            vk::Semaphore make(vk::SemaphoreCreateInfo initializer) 
            { 
                return m_logical.createSemaphore(initializer); 
            }

            template <>
            vk::Fence make(vk::FenceCreateInfo initializer) 
            { 
                return m_logical.createFence(initializer); 
            }

            template <>
            vk::ShaderModule make(vk::ShaderModuleCreateInfo initializer) 
            { 
                return m_logical.createShaderModule(initializer);
            }

            template <>
            vk::PipelineLayout make(vk::PipelineLayoutCreateInfo initializer) 
            { 
                return m_logical.createPipelineLayout(initializer);
            }

            template <>
            vk::Pipeline make(vk::GraphicsPipelineCreateInfo initializer) 
            { 
                return m_logical.createGraphicsPipeline(nullptr, initializer).value; 
            }

            template <>
            vk::DescriptorPool make(vk::DescriptorPoolCreateInfo initializer) 
            { 
                return m_logical.createDescriptorPool(initializer); 
            }

            //Destroyers
            template <class ToDestroy>
            void destroy(ToDestroy &instance) 
            { 
                assert(false && "Device: no Destroy specialisation found for given type"); 
            } // Instance destroyer)

            template <>
            void destroy(vk::Sampler &instance) 
            { 
                m_logical.destroySampler(instance); 
            }

            template <>
            void destroy(vk::ImageView &instance) 
            { 
                m_logical.destroyImageView(instance); 
            }

            template <>
            void destroy(vk::Image &instance) 
            { 
                m_logical.destroyImage(instance); 
            }

            template <>
            void destroy(vk::Buffer &instance) 
            { 
                m_logical.destroyBuffer(instance); 
            }

            template <>
            void destroy(vk::DeviceMemory &instance) 
            { 
                m_logical.freeMemory(instance); 
            }

            template <>
            void destroy(vk::Framebuffer &instance) 
            { 
                m_logical.destroyFramebuffer(instance); 
            }

            template <>
            void destroy(vk::RenderPass &instance) 
            { 
                m_logical.destroyRenderPass(instance); 
            }

            template <>
            void destroy(vk::SwapchainKHR &instance) 
            { 
                m_logical.destroySwapchainKHR(instance); 
            }

            template <>
            void destroy(vk::Pipeline &instance) 
            { 
                m_logical.destroyPipeline(instance); 
            }

            template <>
            void destroy(vk::PipelineLayout &instance) 
            { 
                m_logical.destroyPipelineLayout(instance); 
            }

            template <>
            void destroy(vk::PipelineCache &instance) 
            { 
                m_logical.destroyPipelineCache(instance); 
            }

            template <>
            void destroy(vk::ShaderModule &instance) 
            {
                m_logical.destroyShaderModule(instance); 
            }

            template <>
            void destroy(vk::CommandPool &instance) 
            { 
                m_logical.destroyCommandPool(instance); 
            }

            template <>
            void destroy(vk::SurfaceKHR &instance) 
            { 
                m_vkInstance.destroySurfaceKHR(instance); 
            }

            template <>
            void destroy(vk::DescriptorSetLayout &instance) 
            { 
                m_logical.destroyDescriptorSetLayout(instance); 
            }

            template <>
            void destroy(vk::DescriptorPool &instance) 
            { 
                m_logical.destroyDescriptorPool(instance); 
            }

            template <>
            void destroy(vk::Semaphore &instance) 
            { 
                m_logical.destroySemaphore(instance); 
            }

            template <>
            void destroy(vk::Fence &instance) 
            { 
                m_logical.destroyFence(instance); 
            }

        private:
            void createInstance(const FDeviceCreateInfo& deviceCI);
            void createDebugCallback(const FDeviceCreateInfo& deviceCI);
            void createSurface();
            void createDevice(const FDeviceCreateInfo& deviceCI);

            // Private helpers
            vk::PhysicalDevice getPhysicalDevice(const std::vector<const char*>& deviceExtensions);
            std::vector<vk::PhysicalDevice> getAvaliablePhysicalDevices(const std::vector<const char*>& deviceExtensions);
            bool isDeviceSuitable(const vk::PhysicalDevice &device, const std::vector<const char*>& deviceExtensions);

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