#pragma once
#include <imgui.h>
#include "overlays/OverlayBase.h"

namespace Engine
{
    namespace Resources
    {
        class CResourceManager;
        namespace Material { class CMaterialUI; }
    }

    namespace Core
    {
        class CImage;
        class CVulkanBuffer;
        class CUniformHandler;

        struct FGUIControls
        {
            bool bLMbtn = true;
            bool bRMbtn = true;
            float fMouseX{0.f};
            float fMouseY{0.f};
        };

        class CImguiOverlay
        {
        public:
            CImguiOverlay() = default;
            ~CImguiOverlay();

            void create(std::shared_ptr<Scene::CRenderObject> pRoot, vk::RenderPass& renderPass, uint32_t subpass);
            void reCreate();
            void cleanup();

            void newFrame();

            void update(float deltaTime);
            void drawFrame(vk::CommandBuffer commandBuffer, uint32_t index);

            std::unique_ptr<CVulkanBuffer> &getBuffer(uint32_t index);

        private:
            void baseInitialize();
            void createFontResources(vk::RenderPass& renderPass, uint32_t subpass);

            void initializeWindowBackend();

            void onFocusChange(int focused);
            void onCursorEnter(int enter);
            void onMouseButtonDown(int button, int action, int mods);
            void onMousePositionUpdate(float xpos, float ypos);
            void onMouseScroll(float xpos, float ypos);
            void onKeyboardInput(int key, int scancode, int action, int mods);
            void onInputChar(unsigned int c);
            void onMonitorEvent(int monitor);

            std::shared_ptr<CUniformHandler> m_pUniformHandle;
            std::shared_ptr<CImage> fontTexture;
            std::shared_ptr<Resources::Material::CMaterialUI> fontMaterial;
            std::shared_ptr<CVulkanBuffer> vertexBuffer;
            std::shared_ptr<CVulkanBuffer> indexBuffer;
            int vertexCount{0};
            int indexCount{0};

            std::vector<std::shared_ptr<Overlay::COverlayBase>> m_vOverlays;
            bool bEnabled = true;
            FGUIControls controls;
        };
    }
}