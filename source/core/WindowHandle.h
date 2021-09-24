#pragma once
#include <GLFW/glfw3.h>

namespace window
{
    class WindowHandle
    {
    public:
        WindowHandle();
        ~WindowHandle();

        void Create(uint32_t, uint32_t, const std::string&);
        void ReCreate();
        void Close();

        void ResizeWindow(uint32_t, uint32_t);

        inline void PollEvents() { glfwPollEvents(); }

        inline bool IsShouldClose() { return glfwWindowShouldClose(m_pWindow); }

        inline bool IsFrameBufferResized() { return m_bWasResized; }
        inline void FrameBufferUpdated() { m_bWasResized = false; }

        inline std::pair<int32_t, int32_t> GetSize() { return std::make_pair(m_iWidth, m_iHeight); }

        inline GLFWwindow* GetWindowInstance() 
        {
            return m_pWindow;
        }
    
    private:
        int32_t m_iWidth{800};
        int32_t m_iHeight{600};

        bool m_bWasResized{false};
        GLFWwindow* m_pWindow;

        static void FramebufferResizeCallback(GLFWwindow*, int, int);
        static void KeyCallback(GLFWwindow*, int, int, int, int);
        static void MouseCursorPositionCallback(GLFWwindow*, double, double);
        
    };
}