#include "WindowHandle.h"

namespace Engine
{
    EasyDelegate::TDelegate<void(int, int, int, int)> WindowHandle::KeyCodeCallback;
    EasyDelegate::TDelegate<void(double, double)> WindowHandle::MousePositionCallback;

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
            MousePositionCallback(xpos, ypos);
        }
    }

    WindowHandle::WindowHandle(int iWidth, int iHeight, const char* srWinName) :
    m_iWidth(iWidth), m_iHeight(iHeight), m_srWinName(srWinName)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_pWindow = glfwCreateWindow(m_iWidth, m_iHeight, srWinName, nullptr, nullptr);
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
}