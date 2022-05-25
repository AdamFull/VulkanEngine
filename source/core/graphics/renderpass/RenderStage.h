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
                virtual void create() {}
                virtual void reCreate();
                virtual void rebuild() {}
                virtual void render(vk::CommandBuffer& commandBuffer);
                virtual void cleanup();

                std::unique_ptr<CFramebufferNew>& getFramebuffer() { return pFramebuffer; }
            protected:
                std::unique_ptr<CFramebufferNew> pFramebuffer;
                vk::Extent2D screenExtent;
                bool detectExtent{false};
            };
        }
    }
} 
