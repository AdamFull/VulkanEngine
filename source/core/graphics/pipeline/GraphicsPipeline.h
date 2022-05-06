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

                void create(vk::RenderPass& renderPass, uint32_t subpass) override;

            protected:
                void createPipeline() override;
            };
        }
    }
}