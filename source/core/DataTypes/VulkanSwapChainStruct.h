#pragma once

namespace Engine::SwapChain
{
    struct FVulkanSwapChain
    {
        vk::Format imageFormat;
        vk::Extent2D extent;

        std::vector<vk::Framebuffer> vFramebuffers;
        vk::RenderPass renderPass;

        VulkanImage depth;
        VulkanImage msaa;

        std::vector<vk::Image> vImages;
        std::vector<vk::ImageView> vImageViews;

        size_t framesInFlight{3};

        vk::SwapchainKHR swapChain;

        std::vector<vk::Semaphore> vImageAvailableSemaphores;
        std::vector<vk::Semaphore> vRenderFinishedSemaphores;
        std::vector<vk::Fence> vInFlightFences;
        std::vector<vk::Fence> vImagesInFlight;
        size_t currentFrame{0};
    };
    
}