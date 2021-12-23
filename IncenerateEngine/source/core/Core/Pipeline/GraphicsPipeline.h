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
                GraphicsPipeline() = default;
                GraphicsPipeline(std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapchain);

                void Create(FPipelineCreateInfo createInfo) override;
                void RecreatePipeline(FPipelineCreateInfo createInfo) override;

            protected:
                void CreatePipeline() override;
            };
        }
    }
}