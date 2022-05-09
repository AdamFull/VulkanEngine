#pragma once
#include "graphics/commands/CommandBuffer.h"
#include "graphics/renderpass/Subpass.h"
#include "graphics/renderpass/RenderStage.h"

namespace Engine
{
    namespace Core
    {
        class CRenderSystem : public utl::singleton<CRenderSystem>
        {
        public:
            CRenderSystem() = default;
            ~CRenderSystem();
            void create();
            void reCreate();
            void render();
            void cleanup();

            std::unique_ptr<Render::CRenderStage>& getCurrentStage() { return vStages.at(currentStageIndex); }
            std::unique_ptr<Render::CRenderStage>& getPrevStage() { return vStages.at(currentStageIndex - 1); }

            const size_t getTotalFramesCounted() const { return totalFrameNumberCounter; }

            vk::CommandBuffer& getCurrentCommandBuffer();
        private:
            vk::CommandBuffer beginFrame();
            vk::Result endFrame();

            std::shared_ptr<CCommandBuffer> commandBuffers;
            uint32_t imageIndex{0};
            bool frameStarted{false};
            vk::Extent2D screenExtent{};
            uint32_t currentStageIndex{0};
            bool bIsClean{false};

            size_t totalFrameNumberCounter{0};

            std::vector<std::unique_ptr<Render::CRenderStage>> vStages;
        };
    }
}