#pragma once
#include "RenderPass.hpp"
#include "Framebuffer.hpp"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            struct FRenderCreateInfo;
            struct FRenderProcessInfo;
            class CRenderStage
            {
            public:
                CRenderStage() = default;
                virtual ~CRenderStage() {}
                virtual void create(std::unique_ptr<FRenderCreateInfo>& createInfo) {}
                virtual void reCreate() {}
                virtual void render(std::unique_ptr<FRenderProcessInfo>& renderData) {}
                virtual void cleanup() {}
            protected:
                std::unique_ptr<CRenderPass> pRenderPass;
                std::unique_ptr<CFramebuffer> pFramebuffer;
                vk::Extent2D screenExtent;
            };
        }
    }
} 
