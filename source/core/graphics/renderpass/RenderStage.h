#pragma once
#include "NewFramebuffer.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CRenderStage
            {
            public:
                CRenderStage() = default;
                virtual ~CRenderStage() {}
                virtual void create();
                virtual void reCreate();
                virtual void rebuild() {}
                virtual void render(vk::CommandBuffer& commandBuffer);
                virtual void cleanup();

                scope_ptr<CFramebufferNew>& getFramebuffer(uint32_t index) { return vFramebuffer[index]; }
                scope_ptr<CFramebufferNew>& getCurrentFramebuffer() { return getFramebuffer(framebufferIndex); }
                const uint32_t getFramebufferCount() const { return vFramebuffer.size(); }
            protected:
                std::vector<scope_ptr<CFramebufferNew>> vFramebuffer;
                uint32_t framebufferIndex{0};
                vk::Extent2D screenExtent;
                bool detectExtent{false};
            };
        }
    }
} 
