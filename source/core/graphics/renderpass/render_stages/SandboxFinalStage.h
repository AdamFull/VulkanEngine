#pragma once
#include "graphics/renderpass/RenderStage.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
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