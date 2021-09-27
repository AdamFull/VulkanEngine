#pragma once
#include "VulkanPipeline.h"

namespace Engine
{
    class GraphicsPipeline : public PipelineBase
    {
    public:
        void Create(FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain) override;
        void RecreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain) override;

        void Cleanup(std::unique_ptr<Device>& device) override;
    protected:
        void CreateDescriptorSetLayout(std::unique_ptr<Device>& device);
        void CreateDescriptorPool(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
        void CreatePipelineLayout(std::unique_ptr<Device>& device);
        void CreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
    };
}