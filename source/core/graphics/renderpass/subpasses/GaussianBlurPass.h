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
                void create(std::unique_ptr<FRenderCreateInfo>& createInfo) override;
                void render(std::unique_ptr<FRenderProcessInfo>& renderData) override;
                void cleanup() override;
            private:
                int direction{-1};
            };
        }
    }
}