#include "WindowHandle.h"

namespace Engine
{
    WindowHandle::WindowHandle(int iWidth, int iHeight, const char* srWinName) :
    m_iWidth(iWidth), m_iHeight(iHeight), m_srWinName(srWinName)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_pWindow = glfwCreateWindow(m_iWidth, m_iHeight, srWinName, nullptr, nullptr);
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