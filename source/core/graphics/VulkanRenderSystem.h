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

            void rebuildViewport();

            scope_ptr<Render::CRenderStage>& getStage(uint32_t stageIndex) { return vStages.at(stageIndex); }
            scope_ptr<Render::CRenderStage>& getCurrentStage() { return vStages.at(currentStageIndex); }
            scope_ptr<Render::CRenderStage>& getPrevStage() { return vStages.at(currentStageIndex - 1); }

            std::unordered_map<std::string, ref_ptr<CImage>>& getImages(uint32_t index) { return mImages[index]; }
            std::unordered_map<std::string, ref_ptr<CImage>>& getCurrentImages() { return getImages(imageIndex); }

            const size_t getTotalFramesCounted() const { return totalFrameNumberCounter; }

            vk::CommandBuffer& getCurrentCommandBuffer();
        private:
            vk::CommandBuffer beginFrame();
            vk::Result endFrame();
            void updateFramebufferImages();

            ref_ptr<CCommandBuffer> commandBuffers;
            uint32_t imageIndex{0};
            bool frameStarted{false};
            vk::Extent2D screenExtent{};
            uint32_t currentStageIndex{0};
            bool bIsClean{false};

            size_t totalFrameNumberCounter{0};

            std::vector<scope_ptr<Render::CRenderStage>> vStages;
            std::map<uint32_t, std::unordered_map<std::string, ref_ptr<CImage>>> mImages;
        };
    }
}