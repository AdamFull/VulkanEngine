#pragma once
#include "Pipeline.h"

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            class CComputePipeline : public CPipelineBase
            {
            public:
                CComputePipeline() = default;

                void create() override;
                void recreatePipeline() override;

            protected:
                void createPipeline() override;
            };
        }
    }
}