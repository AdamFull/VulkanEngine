#pragma once
#include "data_types/VulkanSwapChainSipportDetails.h"
#include <util/helpers.hpp>

namespace Engine
{
    namespace Resources
    {
        namespace Material { class CMaterialBase; }
    }
    namespace Core
    {
        class CSwapChain : public utl::singleton<CSwapChain>
        {
        public:
            CSwapChain() = default;
            ~CSwapChain();

            void create();

            vk::Result acquireNextImage(uint32_t *imageIndex);
            vk::Result submitCommandBuffers(const vk::CommandBuffer *commandBuffer, uint32_t *imageIndex, vk::QueueFlagBits queueBit);

            void cleanup();
            void reCreate();

            inline void setFramesInFlight(uint32_t iValue) { m_iFramesInFlight = iValue; }

            inline float getAspectRatio()
            {
                return static_cast<float>(m_extent.width) / static_cast<float>(m_extent.height);
            }

            // Getters
            inline vk::Format getImageFormat() { return m_imageFormat; };
            inline vk::Extent2D getExtent() { return m_extent; }

            void updateCompositionMaterial(vk::CommandBuffer& commandBuffer);

            inline std::vector<vk::Image> &getImages() { return m_vImages; }
            inline std::vector<vk::ImageView> &getImageViews() { return m_vImageViews; }

            inline vk::Extent2D getWindowExtent() { return m_windowExtent; }

            inline vk::SwapchainKHR &GetSwapChain() { return m_swapChain; }

            inline std::vector<vk::Semaphore> &getImageAvailableSemaphores() { return m_vImageAvailableSemaphores; }
            inline std::vector<vk::Semaphore> &getRenderFinishedSemaphores() { return m_vRenderFinishedSemaphores; }
            inline std::vector<vk::Fence> &getInFlightFences() { return m_vInFlightFences; }
            inline size_t getCurrentFrame() { return m_currentFrame; }
            inline uint32_t getFramesInFlight() { return m_iFramesInFlight; }

        private:
            void createSwapChain();
            void createSwapChainImageViews();
            void createSyncObjects();

            vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &);

        private:
            vk::Format m_imageFormat;
            vk::Extent2D m_extent;

            // Swap chain buffer
            std::vector<vk::Image> m_vImages;
            std::vector<vk::ImageView> m_vImageViews;

            // LveDevice &device;
            vk::Extent2D m_windowExtent;

            vk::SwapchainKHR m_swapChain;
            // std::shared_ptr<LveSwapChain> oldSwapChain;

            std::vector<vk::Semaphore> m_vImageAvailableSemaphores;
            std::vector<vk::Semaphore> m_vRenderFinishedSemaphores;
            std::vector<vk::Fence> m_vInFlightFences;
            size_t m_currentFrame{0};
            uint32_t m_iFramesInFlight{2};
        };
    }
}