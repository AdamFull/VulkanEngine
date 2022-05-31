#pragma once
#include <imgui.h>
#include "windows/WindowBase.h"
#include "windows/InspectorWindow.h"
#include "windows/HierarchyWindow.h"
#include "windows/ViewportWindow.h"
#include "windows/WorldSettingsWindow.h"
#include "windows/ContentBrowserWindow.h"
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
            eWorld,
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
            ref_ptr<_Ty> getWindow() {}

            template<>
            ref_ptr<Editor::CHierarchyWindow> getWindow()
            {
                return std::dynamic_pointer_cast<Editor::CHierarchyWindow>(vWindows[EEditorWindowType::eHierarchy]);
            }

            template<>
            ref_ptr<Editor::CInspectorWindow> getWindow()
            {
                return std::dynamic_pointer_cast<Editor::CInspectorWindow>(vWindows[EEditorWindowType::eInspector]);
            }

            template<>
            ref_ptr<Editor::CViewportWindow> getWindow()
            {
                return std::dynamic_pointer_cast<Editor::CViewportWindow>(vWindows[EEditorWindowType::eViewport]);
            }

            template<>
            ref_ptr<Editor::CWorldSettingsWindow> getWindow()
            {
                return std::dynamic_pointer_cast<Editor::CWorldSettingsWindow>(vWindows[EEditorWindowType::eWorld]);
            }

            template<>
            ref_ptr<Editor::CContentBrowserWindow> getWindow()
            {
                return std::dynamic_pointer_cast<Editor::CContentBrowserWindow>(vWindows[EEditorWindowType::eContentBrowser]);
            }

        private:
            void baseInitialize();

            void initializeWindowBackend();

            std::map<EEditorWindowType, ref_ptr<Editor::CWindowBase>> vWindows;
            bool bEnabled = true;
            vk::DescriptorPool descriptorPool{};

            std::map<EEditorWindowType, const char*> vWindowNames{
                {EEditorWindowType::eContentBrowser, "Content browser"},
                {EEditorWindowType::eHierarchy, "Hierarchy"},
                {EEditorWindowType::eInspector, "Inspector"},
                {EEditorWindowType::eViewport, "Viewport"},
                {EEditorWindowType::eWorld, "World"}
            };
        };
    }
}