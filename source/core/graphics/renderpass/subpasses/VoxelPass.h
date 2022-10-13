#pragma once
#include "graphics/renderpass/Subpass.h"

namespace engine
{
    namespace core
    {
        namespace render
        {
            class CVoxelPass : public CSubpass
            {
            public:
                CVoxelPass() = default;
                void create() override;
                void reCreate() override;
                void render(vk::CommandBuffer &commandBuffer) override;
            };
        }
    }
}