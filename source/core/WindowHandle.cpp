#include "WindowHandle.h"

namespace Engine
{
    EasyDelegate::TDelegate<void(int, int, int, int)> WindowHandle::KeyCodeCallback;
    EasyDelegate::TDelegate<void(double, double)> WindowHandle::MousePositionCallback;
    int WindowHandle::m_iWidth{0};
    int WindowHandle::m_iHeight{0};

    void WindowHandle::FramebufferResizeCallback(GLFWwindow* window, int width, int height) 
    {
        auto pThis = reinterpret_cast<WindowHandle*>(glfwGetWindowUserPointer(window));
        pThis->ResizeWindow(width, height);
    }

    void WindowHandle::KeyBoardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if(KeyCodeCallback)
        {
            KeyCodeCallback(key, scancode, action, mods);
        }
    }

    void WindowHandle::MousePositionInputCallback(GLFWwindow* window, double xpos, double ypos)
    {
        if(MousePositionCallback)
        {
            double xmax = static_cast<double>(m_iWidth);
            double ymax = static_cast<double>(m_iHeight);
            MousePositionCallback(xpos, ypos, xmax, ymax);
        }
    }

    WindowHandle::WindowHandle(int iWidth, int iHeight, const char* srWinName) :
    m_srWinName(srWinName)
    {
        m_iWidth = iWidth;
        m_iHeight = iHeight;

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_pWindow = glfwCreateWindow(m_iWidth, m_iHeight, srWinName, nullptr, nullptr);
        glfwSetFramebufferSizeCallback(m_pWindow, &WindowHandle::FramebufferResizeCallback);
        glfwSetKeyCallback(m_pWindow, &WindowHandle::KeyBoardInputCallback);
        glfwSetCursorPosCallback(m_pWindow, &WindowHandle::MousePositionInputCallback);
    }

    void WindowHandle::GetWindowSize(int* width, int* height)
    {
        *width = m_iWidth;
        *height = m_iHeight;
    }

    void WindowHandle::CreateWindowSurface(vk::Instance& instance, vk::SurfaceKHR& surface)
    {
        VkSurfaceKHR rawSurface;
        if (glfwCreateWindowSurface(instance, m_pWindow, nullptr, &rawSurface) != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create window surface!");
        }

        surface = rawSurface;
    }
    
    WindowHandle::~WindowHandle()
    {
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
    }

    void WindowHandle::PollEvents()
    {
        glfwPollEvents();
    }

    bool WindowHandle::ShouldClose()
    {
        return glfwWindowShouldClose(m_pWindow);
    }

    bool WindowHandle::WasResized()
    {
        return m_bWasResized;
    }

    void WindowHandle::ResizeWindow(int width, int height)
    {
        m_iWidth = width;
        m_iHeight = height;
        m_bWasResized = true;
    }
}