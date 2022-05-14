#pragma once
#include "graphics/renderpass/RenderStage.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CShadowMappingStage : public CRenderStage
            {
            public:
                ~CShadowMappingStage();
                void create() override;
                void rebuild() override;
            private:
            };
        }
    }
}