#pragma once
#include "VulkanPipeline.h"

namespace Engine
{
    class GraphicsPipelineDiffuse : public PipelineBase
    {
    public:
        void Create(FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain) override;
        void RecreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain) override;
    protected:
        void CreateDescriptorSetLayout(std::unique_ptr<Device>& device) override;
        void CreateDescriptorPool(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain) override;
        void CreateDescriptorSets(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain) override;
        void CreatePipelineLayout(std::unique_ptr<Device>& device) override;
        void CreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain) override;
    };
}