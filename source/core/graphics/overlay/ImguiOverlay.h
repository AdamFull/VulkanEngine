#pragma once
#include <imgui.h>
#include "overlays/OverlayBase.h"
#include "util/helpers.hpp"

namespace Engine
{
    namespace Resources
    {
        namespace Material { class CMaterialBase; }
    }

    namespace Core
    {
        struct FGUIControls
        {
            bool bLMbtn = true;
            bool bRMbtn = true;
            float fMouseX{0.f};
            float fMouseY{0.f};
        };

        class CImguiOverlay : public utl::singleton<CImguiOverlay>
        {
        public:
            CImguiOverlay() = default;
            ~CImguiOverlay();

            void create(vk::RenderPass& renderPass, uint32_t subpass);
            void reCreate();
            void cleanup();

            void newFrame();
            void drawFrame(vk::CommandBuffer commandBuffer, uint32_t index);

            vk::DescriptorPool& getDescriptorPool() { return descriptorPool; }

        private:
            void baseInitialize();

            void initializeWindowBackend();

            void onFocusChange(int focused);
            void onCursorEnter(int enter);
            void onMouseButtonDown(int button, int action, int mods);
            void onMousePositionUpdate(float xpos, float ypos);
            void onMouseScroll(float xpos, float ypos);
            void onKeyboardInput(int key, int scancode, int action, int mods);
            void onInputChar(unsigned int c);
            void onMonitorEvent(int monitor);

            std::vector<std::shared_ptr<Overlay::COverlayBase>> m_vOverlays;
            bool bEnabled = true;
            FGUIControls controls;
            vk::DescriptorPool descriptorPool{};
        };
    }
}