#pragma once
#include "NewFramebuffer.h"

namespace engine
{
    namespace core
    {
        namespace render
        {
            class CRenderStage
            {
            public:
                CRenderStage() = default;
                virtual ~CRenderStage();
                virtual void create();
                virtual void reCreate();
                virtual void rebuild() {}
                virtual void render(vk::CommandBuffer &commandBuffer);

                utl::scope_ptr<CFramebufferNew> &getFramebuffer(uint32_t index) { return vFramebuffer[index]; }
                utl::scope_ptr<CFramebufferNew> &getCurrentFramebuffer() { return getFramebuffer(framebufferIndex); }
                const size_t getFramebufferCount() const { return vFramebuffer.size(); }

            protected:
                std::vector<utl::scope_ptr<CFramebufferNew>> vFramebuffer;
                uint32_t framebufferIndex{0};
                vk::Extent2D screenExtent;
                bool detectExtent{false};
            };
        }
    }
}