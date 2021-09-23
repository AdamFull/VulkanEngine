#include "WindowHandle.h"

namespace Engine::Window
{
    void Initialize(Main::FVulkanEngine &engine, int width, int height, const char *srWinName)
    {
        engine.window.width = width;
        engine.window.height = height;
        engine.window.srWinName = srWinName;

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        engine.window.pWindow = glfwCreateWindow(width, height, srWinName, nullptr, nullptr);
        //TODO: Think about callbacks
        //glfwSetFramebufferSizeCallback(m_pWindow, &MaiFramebufferResizeCallback);
        //glfwSetKeyCallback(m_pWindow, &MaiKeyBoardInputCallback);
        //glfwSetCursorPosCallback(m_pWindow, &MaiMousePositionInputCallback);
    }

    void Destroy(Main::FVulkanEngine &engine)
    {
        glfwDestroyWindow(engine.window.pWindow);
        glfwTerminate();
    }

    void CreateWindowSurface(Main::FVulkanEngine &engine)
    {
        VkSurfaceKHR rawSurface;
        if (glfwCreateWindowSurface(
            engine.device.vkInstance.get(), 
            engine.window.pWindow,
            nullptr, &rawSurface) != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create window surface!");
        }

        engine.device.surface = rawSurface;
    }

    void PollEvents()
    {
        glfwPollEvents();
    }

    bool ShouldClose(Main::FVulkanEngine &engine)
    {
        return glfwWindowShouldClose(engine.window.pWindow);
    }

    bool WasResized(Main::FVulkanEngine &engine)
    {
        return engine.window.bWasResized;
    }

    void ResizeWindow(Main::FVulkanEngine &engine, int width, int height)
    {
        engine.window.width = width;
        engine.window.height = height;
        engine.window.bWasResized = true;
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