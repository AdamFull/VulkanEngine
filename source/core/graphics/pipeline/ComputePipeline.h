#pragma once
#include "Pipeline.h"

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            /**
             * @brief Compite pipeline class. Just contain data for creation compute pipeline
             * 
             */
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