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

        class ImguiOverlay
        {
        public:
            ImguiOverlay() = default;
            ~ImguiOverlay();

            void Create(std::shared_ptr<Scene::Objects::RenderObject> pRoot, vk::RenderPass& renderPass, uint32_t subpass);
            void ReCreate();
            void Cleanup();

            void NewFrame();

            void Update(float deltaTime);
            void DrawFrame(vk::CommandBuffer commandBuffer, uint32_t index);

            std::unique_ptr<CVulkanBuffer> &GetBuffer(uint32_t index);

        private:
            void BaseInitialize();
            void CreateFontResources(vk::RenderPass& renderPass, uint32_t subpass);

            void InitializeWindowBackend();

            void OnFocusChange(int focused);
            void OnCursorEnter(int enter);
            void OnMouseButtonDown(int button, int action, int mods);
            void OnMousePositionUpdate(float xpos, float ypos);
            void OnMouseScroll(float xpos, float ypos);
            void OnKeyboardInput(int key, int scancode, int action, int mods);
            void OnInputChar(unsigned int c);
            void OnMonitorEvent(int monitor);

            std::shared_ptr<CUniformHandler> m_pUniformHandle;
            std::shared_ptr<CImage> fontTexture;
            std::shared_ptr<Resources::Material::CMaterialUI> fontMaterial;
            std::shared_ptr<CVulkanBuffer> vertexBuffer;
            std::shared_ptr<CVulkanBuffer> indexBuffer;
            int vertexCount{0};
            int indexCount{0};

            std::vector<std::shared_ptr<Overlay::OverlayBase>> m_vOverlays;
            bool bEnabled = true;
            FGUIControls controls;
        };
    }
}