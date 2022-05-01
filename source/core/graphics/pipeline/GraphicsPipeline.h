#pragma once
#include "Pipeline.h"

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            class CGraphicsPipeline : public CPipelineBase
            {
            public:
                CGraphicsPipeline() = default;

                void create() override;
                void recreatePipeline() override;

            protected:
                void createPipeline() override;
            };
        }
    }
}