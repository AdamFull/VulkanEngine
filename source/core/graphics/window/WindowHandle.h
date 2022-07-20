#pragma once
#include "serializer/Serialization.hpp"

namespace engine::core::window
{
    struct FWindowCreateInfo
    {
        uint32_t width{0};
        uint32_t height{0};
        bool fullscreen{true};
        std::string name;
        std::string backend;
    };

    class CWindowHandle
    {
    public:
        CWindowHandle();
        CWindowHandle(FWindowCreateInfo createInfo);
        ~CWindowHandle();

        void wait();

        void resizeWindow(int width, int height);

        inline void pollEvents() { glfwPollEvents(); }

        inline bool isShouldClose() { return glfwWindowShouldClose(pWindow); }

        inline void frameBufferUpdated() { m_bWasResized = false; }

        void createWindowSurface(vk::Instance &instance, const void* pAllocator, VkSurfaceKHR &surface);

        inline std::pair<int32_t, int32_t> getSize() { return std::make_pair(m_iWidth, m_iHeight); }

        inline GLFWwindow *getWindowInstance()
        {
            return pWindow;
        }

        static int32_t m_iWidth;
        static int32_t m_iHeight;
        static bool m_bWasResized;

    private:
        void create(FWindowCreateInfo createInfo);
        void close();
        GLFWwindow *pWindow;
    };

    REGISTER_SERIALIZATION_BLOCK_H(FWindowCreateInfo);
}