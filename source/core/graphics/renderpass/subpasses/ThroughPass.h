#pragma once
#include "graphics/renderpass/Subpass.h"

namespace engine
{
    namespace core
    {
        namespace render
        {
            class CThroughPass : public CSubpass
            {
            public:
                CThroughPass() = default;
                CThroughPass(const std::string& srImage) : srImageSource(srImage) {}
                void create() override;
                void reCreate() override;
                void render(vk::CommandBuffer &commandBuffer) override;
            private:
                std::string srImageSource{};
            };
        }
    }
}