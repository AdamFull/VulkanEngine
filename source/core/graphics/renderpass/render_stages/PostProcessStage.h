#pragma ocne
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
                void reCreate() override;
            private:
            };
        }
    }
}