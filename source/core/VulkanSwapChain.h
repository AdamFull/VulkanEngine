#pragma once
#include "DataTypes/VulkanSwapChainSipportDetails.h"

namespace Engine
{
    class Device;

    struct FSwapChain
    {
        vk::Format imageFormat;
        vk::Format depthFormat;
        vk::Extent2D extent;

        std::vector<vk::Framebuffer> vFramebuffers;
        vk::RenderPass renderPass;

        //Swap chain buffer
        std::vector<vk::Image> vImages;
        std::vector<vk::ImageView> vImageViews;

        //Depth buffer
        vk::Image depthImage;
        vk::DeviceMemory depthImageMemory;
        vk::ImageView depthImageView;

        //MSAA buffer
        vk::Image MSAAImage;
        vk::DeviceMemory MSAAImageMemory;
        vk::ImageView MSAAImageView;

        //LveDevice &device;
        vk::Extent2D windowExtent;

        vk::SwapchainKHR swapChain;
        //std::shared_ptr<LveSwapChain> oldSwapChain;

        std::vector<vk::Semaphore> vImageAvailableSemaphores;
        std::vector<vk::Semaphore> vRenderFinishedSemaphores;
        std::vector<vk::Fence> vInFlightFences;
        std::vector<vk::Fence> vImagesInFlight;
        size_t currentFrame = 0;
        uint32_t iFramesInFlight = 3;
    };

    class SwapChain
    {
    public:
        SwapChain() = default;
        SwapChain(const SwapChain&) = delete;
        void operator=(const SwapChain&) = delete;
        SwapChain(SwapChain&&) = delete;
        SwapChain& operator=(SwapChain&&) = delete;

        void Create(std::unique_ptr<Device>& device);

        vk::Result AcquireNextImage(std::unique_ptr<Device>& device, uint32_t *imageIndex);
        vk::Result SubmitCommandBuffers(std::unique_ptr<Device>& device, const vk::CommandBuffer* commandBuffer, uint32_t *imageIndex);

        void Cleanup(std::unique_ptr<Device>& device);
        void Destroy(std::unique_ptr<Device>& device);
        void ReCreate(std::unique_ptr<Device>& device);

        inline void SetFramesInFlight(uint32_t iValue) { data.iFramesInFlight = iValue; }

        inline float GetAspectRatio() 
        {
            return static_cast<float>(data.extent.width) / static_cast<float>(data.extent.height);
        }

        inline size_t GetImagesSize() { return data.vImages.size(); }
        inline size_t GetFBSize() { return data.vFramebuffers.size(); }

        inline FSwapChain& get() { return data; }
    private:
        void CreateSwapChain(std::unique_ptr<Device>& device);
        void CreateSwapChainImageViews(std::unique_ptr<Device>& device);
        void CreateMSAAResources(std::unique_ptr<Device>& device);
        void CreateDepthResources(std::unique_ptr<Device>& device);
        void CreateRenderPass(std::unique_ptr<Device>& device);
        void CreateFrameBuffers(std::unique_ptr<Device>& device);
        void CreateSyncObjects(std::unique_ptr<Device>& device);

        vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR&);

        FSwapChain data;
    };
}