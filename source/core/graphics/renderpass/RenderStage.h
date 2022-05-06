#pragma once
#include "RenderPass.hpp"
#include "Framebuffer.hpp"

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
                virtual void render(vk::CommandBuffer& commandBuffer);
                virtual void cleanup();

                std::unique_ptr<CRenderPass>& getRenderPass() { return pRenderPass; }
                std::unique_ptr<CFramebuffer>& getFramebuffer() { return pFramebuffer; }
            protected:
                std::unique_ptr<CRenderPass> pRenderPass;
                std::unique_ptr<CFramebuffer> pFramebuffer;
                vk::Extent2D screenExtent;
            };
        }
    }
} 
