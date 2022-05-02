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
                virtual void create(std::shared_ptr<Resources::CResourceManager>& resourceManager, std::shared_ptr<Scene::CRenderObject>& root) {}
                virtual void reCreate() {}
                virtual void render(vk::CommandBuffer& commandBuffer, std::shared_ptr<Scene::CRenderObject>& root) {}
                virtual void cleanup() {}
            protected:
                std::unique_ptr<CRenderPass> pRenderPass;
                std::unique_ptr<CFramebuffer> pFramebuffer;
                vk::Extent2D screenExtent;
            };
        }
    }
} 
