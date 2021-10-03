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

    private:
        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        vk::DescriptorPool m_descriptorPool;
    };
}