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

        static EasyDelegate::TDelegate<void(int, int, int, int)> KeyCodeCallback;
        static EasyDelegate::TDelegate<void(double, double)> MousePositionCallback;
    private:
        static void KeyBoardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MousePositionInputCallback(GLFWwindow* window, double xpos, double ypos);

        const int m_iWidth, m_iHeight;
        std::string m_srWinName;
        GLFWwindow* m_pWindow;
    };
    
}