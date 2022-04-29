#pragma once
#include "Pipeline.h"

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            class ComputePipeline : public PipelineBase
            {
            public:
                ComputePipeline() = default;

                void Create() override;
                void RecreatePipeline() override;

            protected:
                void CreatePipeline() override;
            };
        }
    }
}