#include "WindowHandle.h"
#include "WinCallbacks.h"

namespace Engine
{
    int32_t WindowHandle::m_iWidth{800};
    int32_t WindowHandle::m_iHeight{600};
    bool WindowHandle::m_bWasResized{false};

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

        glfwSetWindowFocusCallback(m_pWindow, &WinCallbacks::WinInputFocusChangeCallback);
        glfwSetCursorEnterCallback(m_pWindow, &WinCallbacks::WinInputCursorEnterCallback);
        glfwSetCursorPosCallback(m_pWindow, &WinCallbacks::WinInputCursorPositionCallback);
        glfwSetMouseButtonCallback(m_pWindow, &WinCallbacks::WinInputMouseButtonCallback);
        glfwSetScrollCallback(m_pWindow, &WinCallbacks::WinInputScrollCallback);
        glfwSetKeyCallback(m_pWindow, &WinCallbacks::WinInputKeyCallback);
        glfwSetCharCallback(m_pWindow, &WinCallbacks::WinInputCharCallback);
        glfwSetMonitorCallback(&WinCallbacks::WinInputMonitorCallback);
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