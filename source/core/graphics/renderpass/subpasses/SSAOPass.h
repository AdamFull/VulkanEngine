#pragma once
#include "graphics/renderpass/Subpass.h"

namespace engine
{
    namespace core
    {
        namespace render
        {
            class CSSAOPass : public CSubpass
            {
            public:
                void create() override;
                void reCreate() override;
                void render(vk::CommandBuffer &commandBuffer) override;
                void cleanup() override;
            private:
                std::array<glm::vec4, SSAO_KERNEL_SIZE> vSSAOKernel;
                utl::ref_ptr<CImage> pNoise;
            };
        }
    }
}