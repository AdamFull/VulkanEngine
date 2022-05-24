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
                void render(vk::CommandBuffer& commandBuffer) override;
                void rebuild() override;
            private:
            };
        }
    }
}