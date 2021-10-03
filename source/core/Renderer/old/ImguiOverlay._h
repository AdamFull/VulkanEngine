#pragma once
#include <imgui.h>

namespace Engine
{
    class Device;
    class SwapChain;
    class WindowHandle;

    class ImguiOverlay
    {
    public:
        void Create(std::unique_ptr<WindowHandle>& winhandle, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
        void Destroy(std::unique_ptr<Device>& device);

        void StartFrame();
        void Render(vk::CommandBuffer commandBuffer);
        void ProcessInterface();

        void CreateDebugOverlay();
        void CreateMenuBar();
        void ShowMenuFile();

    private:
        bool show_demo_window = true;
        float values[90] = {1.f};
        int values_offset{0};
        double refresh_time{0.f};
        vk::DescriptorPool m_descriptorPool;
    };
}