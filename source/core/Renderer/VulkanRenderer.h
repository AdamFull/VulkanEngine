#pragma once

namespace Engine
{
    class Device;
    class SwapChain;

    struct FRenderer
    {
        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> vCommandBuffers;
        uint32_t imageIndex{0};
        bool bFrameStarted;
    };

    class Renderer
    {
    public:
        void Create(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);

        void ReCreate(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
        void Cleanup(std::unique_ptr<Device>& device);
        void Destroy(std::unique_ptr<Device>& device);

        vk::CommandBuffer BeginFrame(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
        vk::Result EndFrame(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);

        void BeginRender(vk::CommandBuffer commandBuffer, std::unique_ptr<SwapChain>& swapchain);
        void EndRender(vk::CommandBuffer commandBuffer, std::unique_ptr<SwapChain>& swapchain);

        //Getters
        inline std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>>& GetCommandBuffers() { return data.vCommandBuffers; }
        inline uint32_t GetImageIndex() { return data.imageIndex; }
        inline bool GetFrameStartFlag() { return data.bFrameStarted; }
        vk::CommandBuffer GetCurrentCommandBuffer() const;
    private:
        void Renderer::CreateCommandBuffers(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
        FRenderer data;
    };
}