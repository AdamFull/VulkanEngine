#pragma once
#include <imgui.h>
#include "VulkanUniform.h"
#include "KeyMapping/KeycodeConfig.h"

namespace Engine
{
    class Device;
    class SwapChain;
    class Texture2D;
    class MaterialUI;
    class VulkanBuffer;
    class WindowHandle;

    struct FGUIControls
    {
        bool bLMbtn = false;
        bool bRMbtn = false;
        float fMouseX{0.f};
        float fMouseY{0.f};
    };

    class ImguiOverlay
    {
    public:
        void Create(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
        void ReCreate(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
        void Cleanup(std::unique_ptr<Device>& device);
        void Destroy(std::unique_ptr<Device>& device);

        void NewFrame();

        void Update(std::unique_ptr<Device>& device, float deltaTime);
        void DrawFrame(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer, uint32_t index);

        void ProcessKeys(EActionKey eKey);
        void ProcessCursor(float fX, float fY);

    private:
        void BaseInitialize();
        void CreateFontResources(std::unique_ptr<Device>& device);
        void CreateResources(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);

        void UpdateControls(float fDeltaTime);

        void CreateDebugOverlay();
        void CreateMenuBar();
        void ShowMenuFile();

        std::unique_ptr<UniformBuffer<FUniformDataUI>> m_pUniform;
        std::shared_ptr<Texture2D> fontTexture;
        std::shared_ptr<MaterialUI> fontMaterial;
        std::unique_ptr<VulkanBuffer> vertexBuffer;
        std::unique_ptr<VulkanBuffer> indexBuffer;

        FGUIControls controls;

        bool show_demo_window = true;
        float values[90] = {1.f};
        int values_offset{0};
        double refresh_time{0.f};
    };
}