#pragma once
#include "DataTypes/VulkanImage.hpp"

namespace Engine
{
    class VulkanDevice;
    class WindowHandle;

    class VulkanSwapChain
    {
    public:
        VulkanSwapChain(std::unique_ptr<VulkanDevice>& device, std::unique_ptr<WindowHandle>& window);
        ~VulkanSwapChain();

        VulkanSwapChain(const VulkanSwapChain&) = delete;
        void operator=(const VulkanSwapChain&) = delete;

        vk::Framebuffer& GetFrameBuffer(int index) { return m_vSwapChainFramebuffers[index]; }
        vk::RenderPass& GetRenderPass() { return m_RenderPass; }
        vk::ImageView& GetImageView(int index) { return m_vSwapChainImages[index].view; }
        size_t GetImageCount() { return m_vSwapChainImages.size(); }
        vk::Format GetSwapChainImageFormat() { return m_SwapChainImageFormat; }
        vk::Extent2D GetSwapChainExtent() { return m_SwapChainExtent; }
        uint32_t GetWidth() { return m_SwapChainExtent.width; }
        uint32_t GetHeight() { return m_SwapChainExtent.height; }

        float extentAspectRatio() {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }
        VkFormat findDepthFormat();

        VkResult acquireNextImage(uint32_t *imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);
    private:
        void CreateSwapChain();
        void CreateImageViews();
        void CreateDepthResources();
        void CreateRenderPass();
        void CreateFramebuffers();
        void CreateSyncObjects();

        vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
        vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
        vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);

        vk::Format m_SwapChainImageFormat;
        vk::Extent2D m_SwapChainExtent;

        std::vector<vk::Framebuffer> m_vSwapChainFramebuffers;
        vk::RenderPass m_RenderPass;

        std::vector<VulkanImage> m_vDepthImages;
        std::vector<VulkanImage> m_vSwapChainImages;

        size_t m_iFramesInFlight{3};

        VulkanDevice& m_Device;
        WindowHandle& m_WinHandle;
    };
}