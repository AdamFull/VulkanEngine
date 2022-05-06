#pragma once
#include "graphics/renderpass/RenderStage.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CPresentFinalStage : public CRenderStage
            {
            public:
                ~CPresentFinalStage();
                void create() override;
            private:
            };
        }
    }
}