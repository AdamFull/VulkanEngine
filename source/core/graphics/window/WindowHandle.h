#pragma once
#include <GLFW/glfw3.h>
#include "serializer/Serialization.hpp"

namespace Engine
{
    namespace Core
    {
        namespace Window
        {
            struct FWindowCreateInfo
            {
                uint32_t width{0};
                uint32_t height{0};
                bool fullscreen{true};
                bool windowed{true};
                std::string name;
                std::string backend;
            };

            class CWindowHandle : public utl::non_copy_movable
            {
            public:
                CWindowHandle();
                ~CWindowHandle();

                void create(FWindowCreateInfo createInfo);
                void wait();
                void close();

                void resizeWindow(int width, int height);

                inline void pollEvents() { glfwPollEvents(); }

                inline bool isShouldClose() { return glfwWindowShouldClose(m_pWindow); }

                inline void frameBufferUpdated() { m_bWasResized = false; }

                void createWindowSurface(vk::Instance &instance, vk::SurfaceKHR &surface);

                inline std::pair<int32_t, int32_t> getSize() { return std::make_pair(m_iWidth, m_iHeight); }

                inline GLFWwindow *getWindowInstance()
                {
                    return m_pWindow;
                }

                static int32_t m_iWidth;
                static int32_t m_iHeight;
                static bool m_bWasResized;

            private:
                GLFWwindow *m_pWindow;
            };

            REGISTER_SERIALIZATION_BLOCK_H(FWindowCreateInfo);
        }
    }
}