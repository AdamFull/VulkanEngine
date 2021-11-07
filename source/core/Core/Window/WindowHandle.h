#pragma once
#include <GLFW/glfw3.h>

namespace Engine
{
    namespace Core
    {
        namespace Window
        {
            struct FWindowCreateInfo
            {
                uint32_t width;
                uint32_t height;
                std::string name;
                std::string backend;
            };

            class WindowHandle
            {
            public:
                WindowHandle();
                ~WindowHandle();

                WindowHandle(const WindowHandle &) = delete;
                void operator=(const WindowHandle &) = delete;
                WindowHandle(WindowHandle &&) = delete;
                WindowHandle &operator=(WindowHandle &&) = delete;

                void Create(FWindowCreateInfo createInfo);
                void Wait();
                void Close();

                void ResizeWindow(uint32_t, uint32_t);

                inline void PollEvents() { glfwPollEvents(); }

                inline bool IsShouldClose() { return glfwWindowShouldClose(m_pWindow); }

                inline void FrameBufferUpdated() { m_bWasResized = false; }

                void CreateWindowSurface(vk::Instance &instance, vk::SurfaceKHR &surface);

                inline std::pair<int32_t, int32_t> GetSize() { return std::make_pair(m_iWidth, m_iHeight); }

                inline GLFWwindow *GetWindowInstance()
                {
                    return m_pWindow;
                }

                static int32_t m_iWidth;
                static int32_t m_iHeight;
                static bool m_bWasResized;

            private:
                GLFWwindow *m_pWindow;
            };
        }
    }
}