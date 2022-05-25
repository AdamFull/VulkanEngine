#pragma once
#include "graphics/renderpass/Subpass.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CFinalCompositionPass : public CSubpass
            {
            public:
                CFinalCompositionPass() = default;
                CFinalCompositionPass(const std::string& blurImage) : blurImageSample(blurImage) {}
                void create() override;
                void reCreate() override;
                void render(vk::CommandBuffer& commandBuffer) override;
                void cleanup() override;
            private:
                std::string blurImageSample{};
            };
        }
    }
}