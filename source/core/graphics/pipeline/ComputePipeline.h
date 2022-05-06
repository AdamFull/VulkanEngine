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

            protected:
                void createPipeline() override;
            };
        }
    }
}