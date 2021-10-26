#pragma once
#include <imgui.h>
#include "Overlays/OverlayBase.h"

namespace Engine
{
    class Device;
    class SwapChain;
    class TextureBase;
    class MaterialUI;
    class VulkanBuffer;
    class UniformBuffer;
    class WindowHandle;

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
        void Create(std::unique_ptr<WindowHandle>& window, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
        void ReCreate(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
        void Cleanup(std::unique_ptr<Device>& device);
        void Destroy(std::unique_ptr<Device>& device);

        void NewFrame();

        void Update(std::unique_ptr<Device>& device, float deltaTime);
        void DrawFrame(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer, uint32_t index);

        std::unique_ptr<VulkanBuffer>& GetBuffer(uint32_t index);

    private:
        void BaseInitialize();
        void CreateFontResources(std::unique_ptr<Device>& device);
        void CreateResources(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);

        void InitializeWindowBackend();

        void OnFocusChange(int focused);
        void OnCursorEnter(int enter);
        void OnMouseButtonDown(int button, int action, int mods);
        void OnMousePositionUpdate(float xpos, float ypos);
        void OnMouseScroll(float xpos, float ypos);
        void OnKeyboardInput(int key, int scancode, int action, int mods);
        void OnInputChar(unsigned int c);
        void OnMonitorEvent(int monitor);

        std::unique_ptr<UniformBuffer> m_pUniform;
        std::shared_ptr<TextureBase> fontTexture;
        std::shared_ptr<MaterialUI> fontMaterial;
        std::unique_ptr<VulkanBuffer> vertexBuffer;
        std::unique_ptr<VulkanBuffer> indexBuffer;
        int vertexCount{0};
        int indexCount{0};

        std::vector<std::shared_ptr<OverlayBase>> m_vOverlays;
        bool bEnabled = true;

        FGUIControls controls;
    };
}