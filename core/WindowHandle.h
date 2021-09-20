#pragma once

namespace Engine
{
    class WindowHandle
    {
    public:
        WindowHandle(int iWidth, int iHeight, const char* srWinName);
        ~WindowHandle();

        WindowHandle(const WindowHandle&) = delete;
        void operator=(const WindowHandle&) = delete;
        WindowHandle(WindowHandle&&) = delete;
        WindowHandle& operator=(WindowHandle&&) = delete;

        void PollEvents();
        bool ShouldClose();
    private:
        const int m_iWidth, m_iHeight;
        std::string m_srWinName;
        GLFWwindow* m_pWindow;
    };
    
}