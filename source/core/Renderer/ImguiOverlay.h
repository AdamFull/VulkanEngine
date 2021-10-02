#pragma once

namespace Engine
{
    class Device;
    class WindowHandle;

    class ImguiOverlay
    {
    public:
        void Create(std::unique_ptr<WindowHandle>& winhandle, std::unique_ptr<Device>& device);
        void Destroy(std::unique_ptr<Device>& device);

        void StartFrame();
        void Render();

    private:
        vk::DescriptorPool m_descriptorPool;
    };
}