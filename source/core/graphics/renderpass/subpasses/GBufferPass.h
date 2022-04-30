#pragma once
#include "graphics/renderpass/Subpass.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CGBufferPass : public CSubpass
            {
            public:
                void create(std::shared_ptr<FRenderCreateInfo> createData) override;
                void render(std::shared_ptr<FRenderProcessInfo> renderData) override;
                void cleanup() override;
            };
        }
    }
}