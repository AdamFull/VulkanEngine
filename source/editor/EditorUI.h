#pragma once
#include <imgui.h>
#include "windows/WindowBase.h"
#include "windows/InspectorWindow.h"
#include "windows/HierarchyWindow.h"
#include "windows/ViewportWindow.h"
#include "windows/WorldSettingsWindow.h"
#include "windows/ContentBrowserWindow.h"
#include "util/helpers.hpp"

namespace engine
{
	namespace editor
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

			void create(vk::RenderPass &renderPass, uint32_t subpass);
			void reCreate();
			void cleanup();

			void newFrame();
			void drawFrame(vk::CommandBuffer commandBuffer, uint32_t index);

			vk::DescriptorPool &getDescriptorPool() { return descriptorPool; }

			template <class _Ty>
			utl::ref_ptr<_Ty> getWindow() {}

			template <>
			utl::ref_ptr<CHierarchyWindow> getWindow()
			{
				return std::dynamic_pointer_cast<CHierarchyWindow>(vWindows[EEditorWindowType::eHierarchy]);
			}

			template <>
			utl::ref_ptr<CInspectorWindow> getWindow()
			{
				return std::dynamic_pointer_cast<CInspectorWindow>(vWindows[EEditorWindowType::eInspector]);
			}

			template <>
			utl::ref_ptr<CViewportWindow> getWindow()
			{
				return std::dynamic_pointer_cast<CViewportWindow>(vWindows[EEditorWindowType::eViewport]);
			}

			template <>
			utl::ref_ptr<CWorldSettingsWindow> getWindow()
			{
				return std::dynamic_pointer_cast<CWorldSettingsWindow>(vWindows[EEditorWindowType::eWorld]);
			}

			template <>
			utl::ref_ptr<CContentBrowserWindow> getWindow()
			{
				return std::dynamic_pointer_cast<CContentBrowserWindow>(vWindows[EEditorWindowType::eContentBrowser]);
			}

		private:
			void baseInitialize();

			void initializeWindowBackend();

			std::map<EEditorWindowType, utl::ref_ptr<CWindowBase>> vWindows;
			bool bEnabled = true;
			vk::DescriptorPool descriptorPool{};

			std::map<EEditorWindowType, const char *> vWindowNames{
				{EEditorWindowType::eContentBrowser, "Content browser"},
				{EEditorWindowType::eHierarchy, "Hierarchy"},
				{EEditorWindowType::eInspector, "Inspector"},
				{EEditorWindowType::eViewport, "Viewport"},
				{EEditorWindowType::eWorld, "World"}};
		};
	}
}