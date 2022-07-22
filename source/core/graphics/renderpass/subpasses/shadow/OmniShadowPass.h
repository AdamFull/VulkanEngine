#pragma once
#include "graphics/renderpass/Subpass.h"

namespace engine
{
    namespace core
    {
        namespace render
        {
            class COmniShadowPass : public CSubpass
            {
            public:
                void create() override;
                void reCreate() override;
                void beforeRender(vk::CommandBuffer& commandBuffer) override;
                void render(vk::CommandBuffer &commandBuffer) override;
            };
        }
    }
}