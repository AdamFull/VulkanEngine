#pragma once
#include "graphics/renderpass/Subpass.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CThroughPass : public CSubpass
            {
            public:
                void create() override;
                void reCreate() override;
                void render(vk::CommandBuffer& commandBuffer) override;
                void cleanup() override;
            };
        }
    }
}