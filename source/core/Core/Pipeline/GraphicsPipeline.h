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
                void Create(FPipelineCreateInfo createInfo) override;
                void RecreatePipeline(FPipelineCreateInfo createInfo) override;

            protected:
                void CreatePipeline() override;
            };
        }
    }
}