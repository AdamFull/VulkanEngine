#pragma once
#include "graphics/renderpass/Subpass.h"

namespace engine
{
    namespace core
    {
        namespace render
        {
            class CShadowPass : public CSubpass
            {
            public:
                void create() override;
                void reCreate() override;
                void render(vk::CommandBuffer &commandBuffer) override;
                void cleanup() override;
            private:
                std::array<glm::mat4, 16> aLightProjections;
            };
        }
    }
}