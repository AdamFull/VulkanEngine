#include "WindowHandle.h"

namespace Engine
{
    EasyDelegate::TDelegate<void(int)> WindowHandle::FocusCallback;
    EasyDelegate::TDelegate<void(unsigned int)> WindowHandle::CharInputCallback;
    EasyDelegate::TDelegate<void(int, int, int, int)> WindowHandle::KeyCodeCallback;
    EasyDelegate::TDelegate<void(double, double, double, double)> WindowHandle::MousePositionCallback;
    EasyDelegate::TDelegate<void(double, double)> WindowHandle::MouseWheelCallback;
    int32_t WindowHandle::m_iWidth{800};
    int32_t WindowHandle::m_iHeight{600};
    bool WindowHandle::m_bWasResized{false};

    void WindowHandle::FocusChangeCallback(GLFWwindow *window, int focus)
    {
        if(FocusCallback)
        {
            FocusCallback(focus);
        }
    }

    void WindowHandle::CharPressedCallback(GLFWwindow *window, unsigned int c)
    {
        if(CharInputCallback)
        {
            CharInputCallback(c);
        }
    }

    void WindowHandle::FramebufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        auto pThis = reinterpret_cast<WindowHandle *>(glfwGetWindowUserPointer(window));
        pThis->ResizeWindow(width, height);
    }

    void WindowHandle::KeyBoardInputCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (KeyCodeCallback)
        {
            KeyCodeCallback(key, scancode, action, mods);
        }
    }

    void WindowHandle::MousePositionInputCallback(GLFWwindow *window, double xpos, double ypos)
    {
        if (MousePositionCallback)
        {
            double xmax = static_cast<double>(m_iWidth);
            double ymax = static_cast<double>(m_iHeight);
            MousePositionCallback(xpos, ypos, xmax, ymax);
        }
    }

    void WindowHandle::MouseButtonInputCallback(GLFWwindow* window, int button, int action, int mods)
    {
        if (KeyCodeCallback)
        {
            KeyCodeCallback(button, 0, action, mods);
        }
    }

    void WindowHandle::MouseWheelInputCallback(GLFWwindow *window, double xpos, double ypos)
    {
        if (MouseWheelCallback)
        {
            MouseWheelCallback(xpos, ypos);
        }
    }

    WindowHandle::WindowHandle()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    WindowHandle::~WindowHandle()
    {
    }

    void WindowHandle::Create(FWindowCreateInfo createInfo)
    {
        ResizeWindow(createInfo.width, createInfo.height);
        FrameBufferUpdated();

        m_pWindow = glfwCreateWindow(createInfo.width, createInfo.height, createInfo.name.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_pWindow, this);
        glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
        //glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetWindowFocusCallback(m_pWindow, &WindowHandle::FocusChangeCallback);
        glfwSetCharCallback(m_pWindow, &WindowHandle::CharPressedCallback);
        glfwSetFramebufferSizeCallback(m_pWindow, &WindowHandle::FramebufferResizeCallback);
        glfwSetKeyCallback(m_pWindow, &WindowHandle::KeyBoardInputCallback);
        glfwSetCursorPosCallback(m_pWindow, &WindowHandle::MousePositionInputCallback);
        glfwSetMouseButtonCallback(m_pWindow, &WindowHandle::MouseButtonInputCallback);
        glfwSetScrollCallback(m_pWindow, &WindowHandle::MouseWheelInputCallback);
    }

    void WindowHandle::CreateWindowSurface(vk::UniqueInstance &instance, vk::SurfaceKHR &surface)
    {
        VkSurfaceKHR rawSurface;
        if (glfwCreateWindowSurface(instance.get(), m_pWindow, nullptr, &rawSurface) != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create window surface!");
        }

        surface = rawSurface;
    }

    void WindowHandle::Wait()
    {
        while (m_iWidth == 0 || m_iHeight == 0)
        {
            glfwGetFramebufferSize(m_pWindow, &m_iWidth, &m_iHeight);
            glfwWaitEvents();
        }
    }

    void WindowHandle::ResizeWindow(uint32_t iWidth, uint32_t iHeight)
    {
        m_iWidth = iWidth;
        m_iHeight = iHeight;
        m_bWasResized = true;
    }

    void WindowHandle::Close()
    {
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
    }
}