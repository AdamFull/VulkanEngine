#pragma once
#include "graphics/renderpass/RenderStage.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CPostProcessStage : public CRenderStage
            {
            public:
                ~CPostProcessStage();
                void create() override;
            private:
            };
        }
    }
}