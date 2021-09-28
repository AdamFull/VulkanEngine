#pragma once

namespace Engine
{
    class Device;
    class SwapChain;
    class PipelineBase;

    struct FRenderer
    {
        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> vCommandBuffers;
        uint32_t imageIndex{0};
        bool bFrameStarted;
    };

    class Renderer
    {
    public:
        void Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain);

        void ReCreate(std::unique_ptr<Device>& device);

        vk::CommandBuffer BeginFrame(std::unique_ptr<Device>& device);
        vk::Result EndFrame(std::unique_ptr<Device>& device);

        void BeginRender(vk::CommandBuffer commandBuffer);
        void EndRender(vk::CommandBuffer commandBuffer);

        //Getters
        inline std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>>& GetCommandBuffers() { return data.vCommandBuffers; }
        inline uint32_t GetImageIndex() { return data.imageIndex; }
    private:
        
        void Renderer::CreateCommandBuffers(std::unique_ptr<Device>& device);

        vk::CommandBuffer GetCurrentCommandBuffer() const;

        std::shared_ptr<SwapChain> m_pSwapChain;

        FRenderer data;
    };
}