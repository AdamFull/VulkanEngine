#pragma once
#include <imgui.h>
#include "windows/WindowBase.h"
#include "util/helpers.hpp"

namespace Engine
{
    namespace Core
    {
        class CEditorUI : public utl::singleton<CEditorUI>
        {
        public:
            CEditorUI() = default;
            ~CEditorUI();

            void create(vk::RenderPass& renderPass, uint32_t subpass);
            void reCreate();
            void cleanup();

            void newFrame();
            void drawFrame(vk::CommandBuffer commandBuffer, uint32_t index);

            vk::DescriptorPool& getDescriptorPool() { return descriptorPool; }

        private:
            void baseInitialize();

            void initializeWindowBackend();

            std::vector<std::shared_ptr<Editor::CWindowBase>> vWindows;
            bool bEnabled = true;
            vk::DescriptorPool descriptorPool{};
        };
    }
}