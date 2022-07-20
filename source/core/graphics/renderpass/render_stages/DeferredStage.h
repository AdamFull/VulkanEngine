#pragma once
#include "graphics/renderpass/RenderStage.h"

namespace engine
{
    namespace core
    {
        namespace render
        {
            class CDeferredStage : public CRenderStage
            {
            public:
                void create() override;
                void rebuild() override;
            };
        }
    }
}