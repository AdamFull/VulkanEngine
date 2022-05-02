#pragma once
#include "graphics/commands/CommandBuffer.h"
#include "graphics/renderpass/Subpass.h"
#include "graphics/renderpass/RenderStage.h"

namespace Engine
{
    namespace Resources { class ResourceManager; }
    namespace Core
    {
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

            std::shared_ptr<CCommandBuffer> commandBuffers;
            uint32_t imageIndex{0};
            bool frameStarted{false};
            vk::Extent2D screenExtent{};
            
            std::unique_ptr<Render::FRenderCreateInfo> creationBuffer;
            std::unique_ptr<Render::FRenderProcessInfo> renderData;

            std::vector<std::unique_ptr<Render::CRenderStage>> vStages;
        };
    }
}