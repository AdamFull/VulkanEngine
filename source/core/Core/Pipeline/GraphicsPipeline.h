#pragma once
#include "VulkanPipeline.h"

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            class GraphicsPipeline : public PipelineBase
            {
            public:
                void Create(FPipelineCreateInfo createInfo, std::unique_ptr<Device> &device, std::unique_ptr<SwapChain> &swapchain) override;
                void RecreatePipeline(FPipelineCreateInfo createInfo, std::unique_ptr<Device> &device, std::unique_ptr<SwapChain> &swapchain) override;

            protected:
                void CreatePipeline(std::unique_ptr<Device> &device, std::unique_ptr<SwapChain> &swapchain) override;
            };
        }
    }
}