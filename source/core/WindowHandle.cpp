#include "WindowHandle.h"
#include "Engine.hpp"

using namespace window;

void WindowHandle::FramebufferResizeCallback(GLFWwindow* window, int width, int height) 
{
    auto pThis = reinterpret_cast<WindowHandle*>(glfwGetWindowUserPointer(window));
    pThis->ResizeWindow(width, height);
}

void WindowHandle::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    
}

void WindowHandle::MouseCursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
}

WindowHandle::WindowHandle()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

WindowHandle::~WindowHandle()
{
}

void WindowHandle::Create(uint32_t iWidth, uint32_t iHeight, const std::string& srWinName)
{
    ResizeWindow(iWidth, iHeight);
    FrameBufferUpdated();

    m_pWindow = glfwCreateWindow(iWidth, iHeight, srWinName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_pWindow, this);
    glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS , GLFW_TRUE);
    glfwSetFramebufferSizeCallback(m_pWindow, &WindowHandle::FramebufferResizeCallback);
    //glfwSetKeyCallback(m_pWindow, &WindowHandle::KeyCallback);
    //glfwSetCursorPosCallback(m_pWindow, &WindowHandle::MouseCursorPositionCallback);
}

void WindowHandle::ReCreate()
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