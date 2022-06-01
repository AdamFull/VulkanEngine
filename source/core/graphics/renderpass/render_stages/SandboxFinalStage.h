#pragma once
#include "graphics/renderpass/RenderStage.h"

namespace engine
{
    namespace core
    {
        namespace render
        {
            class CSandboxFinalStage : public CRenderStage
            {
            public:
                ~CSandboxFinalStage();
                void create() override;

            private:
            };
        }
    }
}