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

        vk::Framebuffer& GetFrameBuffer(int index);
        vk::RenderPass& GetRenderPass();
        vk::ImageView& GetImageView(int index);
        size_t GetImageCount();
        vk::Format GetSwapChainImageFormat();
        vk::Extent2D GetSwapChainExtent();
        uint32_t GetWidth();
        uint32_t GetHeight();

        float GetExtentAspectRatio();

        vk::Format FindDepthFormat();

        vk::Result AcquireNextImage(uint32_t *imageIndex);
        vk::Result SubmitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);
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