#pragma once
#include <imgui.h>
#include "windows/WindowBase.h"
#include "windows/InspectorWindow.h"
#include "windows/HierarchyWindow.h"
#include "windows/ViewportWindow.h"
#include "util/helpers.hpp"

namespace Engine
{
    namespace Core
    {
        enum class EEditorWindowType
        {
            eHierarchy,
            eInspector,
            eViewport,
            eContentBrowser
        };

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

            template<class _Ty>
            std::shared_ptr<_Ty> getWindow() {}

            template<>
            std::shared_ptr<Editor::CHierarchyWindow> getWindow()
            {
                return std::dynamic_pointer_cast<Editor::CHierarchyWindow>(vWindows[EEditorWindowType::eHierarchy]);
            }

            template<>
            std::shared_ptr<Editor::CInspectorWindow> getWindow()
            {
                return std::dynamic_pointer_cast<Editor::CInspectorWindow>(vWindows[EEditorWindowType::eInspector]);
            }

            template<>
            std::shared_ptr<Editor::CViewportWindow> getWindow()
            {
                return std::dynamic_pointer_cast<Editor::CViewportWindow>(vWindows[EEditorWindowType::eViewport]);
            }

        private:
            void baseInitialize();

            void initializeWindowBackend();

            std::map<EEditorWindowType, std::shared_ptr<Editor::CWindowBase>> vWindows;
            bool bEnabled = true;
            vk::DescriptorPool descriptorPool{};
        };
    }
}