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
                void create(std::unique_ptr<FRenderCreateInfo>& createInfo) override;
                void render(std::unique_ptr<FRenderProcessInfo>& renderData) override;
                void cleanup() override;
            };
        }
    }
}