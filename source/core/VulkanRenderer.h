#pragma once

namespace Engine
{
    class Device;
    class SwapChain;
    class GraphicsPipeline;
    class UniformBuffer;

    struct FRenderer
    {
        std::vector<vk::DescriptorSet> vDescriptorSets;
        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> vCommandBuffers;
        uint32_t imageIndex{0};
        bool bFrameStarted;
    };

    class Renderer
    {
    public:
        void Create(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, std::unique_ptr<GraphicsPipeline>& pipeline, std::unique_ptr<UniformBuffer>& uniform);

        vk::CommandBuffer BeginFrame(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, std::unique_ptr<GraphicsPipeline>& pipeline, std::unique_ptr<UniformBuffer>& uniform);
        void EndFrame(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, std::unique_ptr<GraphicsPipeline>& pipeline, std::unique_ptr<UniformBuffer>& uniform);

        void BeginRender(vk::CommandBuffer commandBuffer, std::unique_ptr<SwapChain>& swapchain);
        void EndRender(vk::CommandBuffer commandBuffer);

        vk::DescriptorSet Renderer::GetCurrentDescriptorSets() const;

        void CleanupSwapChain(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);

        //Getters
        inline std::vector<vk::DescriptorSet>& GetDescriptorSets() { return data.vDescriptorSets; }
        inline std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>>& GetCommandBuffers() { return data.vCommandBuffers; }
        inline uint32_t GetImageIndex() { return data.imageIndex; }
    private:
        void RecreateSwapChain(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, std::unique_ptr<GraphicsPipeline>& pipeline, std::unique_ptr<UniformBuffer>& uniform);
        void Renderer::CreateCommandBuffers(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, std::unique_ptr<GraphicsPipeline>& pipeline, std::unique_ptr<UniformBuffer>& uniform);
        void CreateDescriptorSets(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, std::unique_ptr<GraphicsPipeline>& pipeline, std::unique_ptr<UniformBuffer>& uniform);

        vk::CommandBuffer GetCurrentCommandBuffer() const;

        FRenderer data;
    };
}