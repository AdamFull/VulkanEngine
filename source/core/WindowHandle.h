#pragma once
#include <GLFW/glfw3.h>

namespace Engine
{
    class WindowHandle
    {
    public:
        WindowHandle();
        ~WindowHandle();

        WindowHandle(const WindowHandle&) = delete;
        void operator=(const WindowHandle&) = delete;
        WindowHandle(WindowHandle&&) = delete;
        WindowHandle& operator=(WindowHandle&&) = delete;

        void Create(uint32_t, uint32_t, const std::string&);
        void ReCreate();
        void Close();

        void ResizeWindow(uint32_t, uint32_t);

        inline void PollEvents() { glfwPollEvents(); }

        inline bool IsShouldClose() { return glfwWindowShouldClose(m_pWindow); }

        inline void FrameBufferUpdated() { m_bWasResized = false; }

        void CreateWindowSurface(vk::UniqueInstance& instance, vk::SurfaceKHR& surface);

        inline std::pair<int32_t, int32_t> GetSize() { return std::make_pair(m_iWidth, m_iHeight); }

        inline GLFWwindow* GetWindowInstance() 
        {
            return m_pWindow;
        }

        static EasyDelegate::TDelegate<void(int, int, int, int)> KeyCodeCallback;
        static EasyDelegate::TDelegate<void(double, double, double, double)> MousePositionCallback;
        static int32_t m_iWidth;
        static int32_t m_iHeight;
        static bool m_bWasResized;
    private:
        GLFWwindow* m_pWindow;

        static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);
        static void KeyBoardInputCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void MousePositionInputCallback(GLFWwindow *window, double xpos, double ypos);
        
    };
}