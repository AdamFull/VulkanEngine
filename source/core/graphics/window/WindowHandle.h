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

    class CWindowHandle : public utl::singleton<CWindowHandle>
    {
    public:
        CWindowHandle();
        ~CWindowHandle();

        void create(FWindowCreateInfo createInfo);
        void wait();
        void close();

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
        GLFWwindow *pWindow;
    };

    REGISTER_SERIALIZATION_BLOCK_H(FWindowCreateInfo);
}