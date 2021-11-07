#pragma once
#include <imgui.h>
#include "Overlays/OverlayBase.h"

namespace Engine
{
    namespace Resources
    {
        namespace Texture { class TextureBase; }
        namespace Material { class MaterialUI; }
    }

    namespace Core
    {
        class Device;
        class SwapChain;
        class VulkanBuffer;
        class UniformBuffer;

        namespace Window
        {
            class WindowHandle;
        }

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
            ImguiOverlay(std::shared_ptr<Window::WindowHandle> window, std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapchain);
            ~ImguiOverlay();

            void Create();
            void ReCreate();
            void Cleanup();

            void NewFrame();

            void Update(float deltaTime);
            void DrawFrame(vk::CommandBuffer commandBuffer, uint32_t index);

            std::unique_ptr<VulkanBuffer> &GetBuffer(uint32_t index);

        private:
            void BaseInitialize();
            void CreateFontResources();
            void CreateResources();

            void InitializeWindowBackend();

            void OnFocusChange(int focused);
            void OnCursorEnter(int enter);
            void OnMouseButtonDown(int button, int action, int mods);
            void OnMousePositionUpdate(float xpos, float ypos);
            void OnMouseScroll(float xpos, float ypos);
            void OnKeyboardInput(int key, int scancode, int action, int mods);
            void OnInputChar(unsigned int c);
            void OnMonitorEvent(int monitor);

            std::shared_ptr<UniformBuffer> m_pUniform;
            std::shared_ptr<Resources::Texture::TextureBase> fontTexture;
            std::shared_ptr<Resources::Material::MaterialUI> fontMaterial;
            std::shared_ptr<VulkanBuffer> vertexBuffer;
            std::shared_ptr<VulkanBuffer> indexBuffer;
            int vertexCount{0};
            int indexCount{0};

            std::vector<std::shared_ptr<Overlay::OverlayBase>> m_vOverlays;
            bool bEnabled = true;

            std::shared_ptr<Window::WindowHandle> m_window;
            std::shared_ptr<Device> m_device;
            std::shared_ptr<SwapChain> m_swapchain;

            FGUIControls controls;
        };
    }
}