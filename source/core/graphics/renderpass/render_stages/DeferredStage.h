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
                void create(std::unique_ptr<FRenderCreateInfo>& createInfo) override;
                void reCreate() override;
                void render(std::unique_ptr<FRenderProcessInfo>& renderData) override;
                void cleanup() override;
            private:
            };
        }
    }
}