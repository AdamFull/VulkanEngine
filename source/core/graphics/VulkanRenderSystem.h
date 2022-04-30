#pragma once
#include "graphics/commands/CommandBuffer.h"
#include "graphics/renderpass/Framebuffer.hpp"

namespace Engine
{
    namespace Resources { class ResourceManager; }
    namespace Core
    {
        namespace Render 
        { 
            class CRenderPass; 
            struct FRenderCreateInfo;
            struct FRenderProcessInfo;
        }
        namespace Scene { namespace Objects { class RenderObject; } }
        class RenderSystem
        {
        public:
            RenderSystem() = default;
            ~RenderSystem();
            void create(std::shared_ptr<Resources::ResourceManager> resourceManager, std::shared_ptr<Scene::Objects::RenderObject> root);
            void reCreate();
            void render(std::shared_ptr<Scene::Objects::RenderObject> root);
            void cleanup();

            vk::CommandBuffer& getCurrentCommandBuffer();
        private:
            vk::CommandBuffer& beginFrame();
            vk::Result endFrame();

            std::shared_ptr<CommandBuffer> commandBuffers;
            uint32_t imageIndex{0};
            bool frameStarted{false};
            vk::Extent2D screenExtent{};
            
            std::shared_ptr<Render::FRenderCreateInfo> creationBuffer;
            std::shared_ptr<Render::FRenderProcessInfo> renderData;

            std::shared_ptr<Render::CRenderPass> renderPass;
            std::shared_ptr<Render::CFramebuffer> framebuffer;
        };
    }
}