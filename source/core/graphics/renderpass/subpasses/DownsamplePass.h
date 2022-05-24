#pragma once
#include "graphics/renderpass/Subpass.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CDownsamplePass : public CSubpass
            {
            public:
                CDownsamplePass() = default;
                void create() override;
                void reCreate() override;
                void render(vk::CommandBuffer& commandBuffer) override;
                void cleanup() override;
            private:
                int direction{-1};
                std::string imageReferenceName{""};
            };
        }
    }
}