#pragma once

namespace Engine
{
    class WindowHandle
    {
    public:
        WindowHandle() = default;
        WindowHandle(int iWidth, int iHeight, const char* srWinName);
        ~WindowHandle();

        WindowHandle(const WindowHandle&) = delete;
        void operator=(const WindowHandle&) = delete;
        WindowHandle(WindowHandle&&) = delete;
        WindowHandle& operator=(WindowHandle&&) = delete;

        void CreateWindowSurface(vk::Instance& instance, vk::SurfaceKHR& surface);

        void PollEvents();
        bool ShouldClose();
    private:
        const int m_iWidth, m_iHeight;
        std::string m_srWinName;
        GLFWwindow* m_pWindow;
    };
    
}