#pragma once
#include "graphics/renderpass/Subpass.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CGaussianBlurPass : public CSubpass
            {
            public:
                CGaussianBlurPass() = default;
                explicit CGaussianBlurPass(int direction) : direction(direction) {}
                void create() override;
                void render(vk::CommandBuffer& commandBuffer) override;
                void cleanup() override;
            private:
                int direction{-1};
            };
        }
    }
}