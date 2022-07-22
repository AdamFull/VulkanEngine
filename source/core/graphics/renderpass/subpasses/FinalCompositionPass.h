#pragma once
#include "graphics/renderpass/Subpass.h"

namespace engine
{
    namespace core
    {
        namespace render
        {
            class CFinalCompositionPass : public CSubpass
            {
            public:
                CFinalCompositionPass() = default;
                CFinalCompositionPass(const std::string &blurImage) : blurImageSample(blurImage) {}
                void create() override;
                void reCreate() override;
                void render(vk::CommandBuffer &commandBuffer) override;

            private:
                std::string blurImageSample{};
            };
        }
    }
}