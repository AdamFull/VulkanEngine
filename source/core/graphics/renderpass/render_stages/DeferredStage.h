#pragma once
#include "graphics/renderpass/RenderStage.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CDeferredStage : public CRenderStage
            {
            public:
                ~CDeferredStage();
                void create() override;
                void reCreate() override;
            private:
            };
        }
    }
}