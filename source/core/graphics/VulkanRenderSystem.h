#pragma once
#include "graphics/commands/CommandBuffer.h"
#include "graphics/renderpass/Subpass.h"
#include "graphics/renderpass/RenderStage.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene { class CRenderObject; }
        class CRenderSystem : public utl::singleton<CRenderSystem>
        {
        public:
            CRenderSystem() = default;
            ~CRenderSystem();
            void create(std::shared_ptr<Scene::CRenderObject> root);
            void reCreate();
            void render(std::shared_ptr<Scene::CRenderObject> root);
            void cleanup();

            std::unique_ptr<Render::CRenderStage>& getCurrentStage() { return vStages.at(currentStageIndex); }

            vk::CommandBuffer& getCurrentCommandBuffer();
        private:
            vk::CommandBuffer& beginFrame();
            vk::Result endFrame();

            std::shared_ptr<CCommandBuffer> commandBuffers;
            uint32_t imageIndex{0};
            bool frameStarted{false};
            vk::Extent2D screenExtent{};
            uint32_t currentStageIndex{0};

            std::vector<std::unique_ptr<Render::CRenderStage>> vStages;
        };
    }
}