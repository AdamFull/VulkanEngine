#include "WindowHandle.h"

namespace Engine::Window
{
    void Initialize(Main::FVulkanRenderInstance &renderInstance, int width, int height, const char *srWinName)
    {
        renderInstance.window.width = width;
        renderInstance.window.height = height;
        renderInstance.window.srWinName = srWinName;

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        renderInstance.window.pWindow = glfwCreateWindow(width, height, srWinName, nullptr, nullptr);
        //TODO: Think about callbacks
        //glfwSetFramebufferSizeCallback(m_pWindow, &MaiFramebufferResizeCallback);
        //glfwSetKeyCallback(m_pWindow, &MaiKeyBoardInputCallback);
        //glfwSetCursorPosCallback(m_pWindow, &MaiMousePositionInputCallback);
    }

    void Destroy(Main::FVulkanRenderInstance &renderInstance)
    {
        glfwDestroyWindow(renderInstance.window.pWindow);
        glfwTerminate();
    }

    void CreateWindowSurface(Main::FVulkanRenderInstance &renderInstance)
    {
        VkSurfaceKHR rawSurface;
        if (glfwCreateWindowSurface(
            renderInstance.device.vkInstance.get(), 
            renderInstance.window.pWindow,
            nullptr, &rawSurface) != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create window surface!");
        }

        renderInstance.device.surface = rawSurface;
    }

    void PollEvents()
    {
        glfwPollEvents();
    }

    bool ShouldClose(Main::FVulkanRenderInstance &renderInstance)
    {
        return glfwWindowShouldClose(renderInstance.window.pWindow);
    }

    bool WasResized(Main::FVulkanRenderInstance &renderInstance)
    {
        return renderInstance.window.bWasResized;
    }

    void ResizeWindow(Main::FVulkanRenderInstance &renderInstance, int width, int height)
    {
        renderInstance.window.width = width;
        renderInstance.window.height = height;
        renderInstance.window.bWasResized = true;
    }

    void FramebufferResizeCallback(GLFWwindow* window, int width, int height) 
    {
        /*auto pThis = reinterpret_cast<WindowHandle*>(glfwGetWindowUserPointer(window));
        pThis->ResizeWindow(width, height);*/
    }

    void KeyBoardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        /*if(KeyCodeCallback)
        {
            KeyCodeCallback(key, scancode, action, mods);
        }*/
    }

    void MousePositionInputCallback(GLFWwindow* window, double xpos, double ypos)
    {
        /*if(MousePositionCallback)
        {
            double xmax = static_cast<double>(m_iWidth);
            double ymax = static_cast<double>(m_iHeight);
            MousePositionCallback(xpos, ypos, xmax, ymax);
        }*/
    }
}