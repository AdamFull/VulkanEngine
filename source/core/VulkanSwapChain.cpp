#include "VulkanSwapChain.h"
#include "VulkanDevice.h"
#include "WindowHandle.h"

namespace Engine 
{
    VulkanSwapChain::VulkanSwapChain(std::unique_ptr<VulkanDevice>& device, std::unique_ptr<WindowHandle>& window)
    {
        m_Device = device.get();
        m_WinHandle = window.get();
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        for (auto& image : m_vSwapChainImages) 
        {
            image.Destroy(m_Device.GetLogical());
        }
        m_vSwapChainImages.clear();

        if (m_SwapChain != nullptr) 
        {
            m_Device.GetLogical()->destroySwapchainKHR(m_SwapChain);
            m_SwapChain = nullptr;
        }

        for (auto& image : m_vDepthImages) 
        {
            image.Destroy(m_Device.GetLogical());
        }
        m_vDepthImages.clear();

        for (auto& framebuffer : m_vSwapChainFramebuffers) 
        {
            m_Device.GetLogical()->destroyFramebuffer(framebuffer);
        }

        m_Device.GetLogical()->destroyRenderPass(m_RenderPass);

        // cleanup synchronization objects
        for (size_t i = 0; i < m_iFramesInFlight; i++) 
        {
            m_Device.GetLogical()->destroySemaphore(m_vRenderFinishedSemaphores[i]);
            m_Device.GetLogical()->destroySemaphore(m_vImageAvailableSemaphores[i]);
            m_Device.GetLogical()->destroyFence(m_vInFlightFences[i]);
        }
    }

    vk::Framebuffer& VulkanSwapChain::GetFrameBuffer(int index) 
    { 
        return m_vSwapChainFramebuffers[index];
    }

    vk::RenderPass& VulkanSwapChain::GetRenderPass() 
    { 
        return m_RenderPass;
    }

    vk::ImageView& VulkanSwapChain::GetImageView(int index) 
    { 
        return m_vSwapChainImages[index].view;
    }

    size_t VulkanSwapChain::GetImageCount() 
    { 
        return m_vSwapChainImages.size();
    }

    vk::Format VulkanSwapChain::GetSwapChainImageFormat() 
    { 
        return m_SwapChainImageFormat;
    }

    vk::Extent2D VulkanSwapChain::GetSwapChainExtent() 
    { 
        return m_SwapChainExtent;
    }

    uint32_t VulkanSwapChain::GetWidth() 
    { 
        return m_SwapChainExtent.width;
    }

    uint32_t VulkanSwapChain::GetHeight() 
    { 
        return m_SwapChainExtent.height;
    }

    float VulkanSwapChain::GetExtentAspectRatio() 
    {
        return static_cast<float>(m_SwapChainExtent.width) / static_cast<float>(m_SwapChainExtent.height);
    }

     vk::Format VulkanSwapChain::FindDepthFormat()
     {

     }

     vk::Result VulkanSwapChain::AcquireNextImage(uint32_t *imageIndex)
     {
     }

     vk::Result VulkanSwapChain::SubmitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex)
     {
     }

    void VulkanSwapChain::CreateSwapChain()
    {

    }

    void VulkanSwapChain::CreateImageViews()
    {

    }
    
    void VulkanSwapChain::CreateDepthResources()
    {

    }
    
    void VulkanSwapChain::CreateRenderPass()
    {

    }
    
    void VulkanSwapChain::CreateFramebuffers()
    {

    }
    
    void VulkanSwapChain::CreateSyncObjects()
    {

    }

    vk::SurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
    {
        if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
        {
            return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
        }

        for (const auto &availableFormat : availableFormats)
        {
            if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }
    
    vk::PresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes)
    {
        vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

        for (const auto &availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox)
            {
                return availablePresentMode;
            }
            else if (availablePresentMode == vk::PresentModeKHR::eImmediate)
            {
                bestMode = availablePresentMode;
            }
        }

        return bestMode;
    }
    
    vk::Extent2D VulkanSwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            m_WinHandle.GetWindowSize(width, height);
            vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }
    
}