#pragma once
#include "DataTypes/VulkanSwapChainSipportDetails.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material { class MaterialBase; }
    }
    namespace Core
    {
        class Image;
        class SwapChain
        {
        public:
            SwapChain() = default;
            ~SwapChain();

            SwapChain(const SwapChain &) = delete;
            void operator=(const SwapChain &) = delete;
            SwapChain(SwapChain &&) = delete;
            SwapChain &operator=(SwapChain &&) = delete;

            void Create();

            vk::Result AcquireNextImage(uint32_t *imageIndex);
            vk::Result SubmitCommandBuffers(const vk::CommandBuffer *commandBuffer, uint32_t *imageIndex);

            void Cleanup();
            void ReCreate();

            inline void SetFramesInFlight(uint32_t iValue) { m_iFramesInFlight = iValue; }

            inline float GetAspectRatio()
            {
                return static_cast<float>(m_extent.width) / static_cast<float>(m_extent.height);
            }

            // Getters
            inline vk::Format GetImageFormat() { return m_imageFormat; };
            inline vk::Format GetDepthFormat() { return m_depthFormat; }
            inline vk::Extent2D GetExtent() { return m_extent; }

            inline std::vector<vk::Framebuffer> &GetFramebuffers() { return m_vFramebuffers; }
            inline vk::RenderPass &GetRenderPass() { return m_renderPass; }

            void UpdateCompositionMaterial(vk::CommandBuffer& commandBuffer);

            inline std::vector<vk::Image> &GetImages() { return m_vImages; }
            inline std::vector<vk::ImageView> &GetImageViews() { return m_vImageViews; }

            inline vk::Image &GetDepthImage() { return m_depthImage; }
            inline vk::DeviceMemory &GetDepthImageMemory() { return m_depthImageMemory; }
            inline vk::ImageView &GetDepthImageView() { return m_depthImageView; }

            inline vk::Extent2D GetWindowExtent() { return m_windowExtent; }

            inline vk::SwapchainKHR &GetSwapChain() { return m_swapChain; }

            inline std::vector<vk::Semaphore> &GetImageAvailableSemaphores() { return m_vImageAvailableSemaphores; }
            inline std::vector<vk::Semaphore> &GetRenderFinishedSemaphores() { return m_vRenderFinishedSemaphores; }
            inline std::vector<vk::Fence> &GetInFlightFences() { return m_vInFlightFences; }
            inline size_t GetCurrentFrame() { return m_currentFrame; }
            inline uint32_t GetFramesInFlight() { return m_iFramesInFlight; }

        private:
            void CreateSwapChain();
            void CreateSwapChainImageViews();
            void CreateDepthResources();
            void CreateRenderPass();
            void CreateFrameBuffers();
            void CreateSyncObjects();

            vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &);

        private:
            vk::Format m_imageFormat;
            vk::Format m_depthFormat;
            vk::Extent2D m_extent;

            std::vector<vk::Framebuffer> m_vFramebuffers;
            vk::RenderPass m_renderPass;

            // Swap chain buffer
            std::vector<vk::Image> m_vImages;
            std::vector<vk::ImageView> m_vImageViews;

            // Depth buffer
            vk::Image m_depthImage;
            vk::DeviceMemory m_depthImageMemory;
            vk::ImageView m_depthImageView;

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