#pragma once
#include "graphics/commands/CommandBuffer.h"

namespace Engine
{
    namespace Core
    {
        namespace Render { class CRenderPass; }
        class CRenderer
        {
        public:
            CRenderer() = default;
            ~CRenderer();
            void create();
            void reCreate();
            void render();
            void cleanup();
        private:
            vk::CommandBuffer& beginFrame();
            vk::Result endFrame();

            std::shared_ptr<CommandBuffer> commandBuffers;
            uint32_t imageIndex{0};
            bool frameStarted{false};
            vk::Extent2D screenExtent{};
            std::vector<std::shared_ptr<Render::CRenderPass>> vRenderPasses;
        };
    }
}