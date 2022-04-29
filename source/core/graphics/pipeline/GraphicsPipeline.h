#pragma once
#include "Pipeline.h"

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

                void Create() override;
                void RecreatePipeline() override;

            protected:
                void CreatePipeline() override;
            };
        }
    }
}