#include "WindowHandle.h"

namespace Engine::Window
{
    void FramebufferResizeCallback(GLFWwindow* window, int width, int height) 
    {
        *pWidth = width;
        *pHeight = height;
        *pWindowResized = true;
    }

    void KeyBoardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if(KeyCodeCallback)
        {
            KeyCodeCallback(key, scancode, action, mods);
        }
    }

    void MousePositionInputCallback(GLFWwindow* window, double xpos, double ypos)
    {
        if(MousePositionCallback)
        {
            double xmax = static_cast<double>(*pWidth);
            double ymax = static_cast<double>(*pHeight);
            MousePositionCallback(xpos, ypos, xmax, ymax);
        }
    }

    void Initialize(Main::FVulkanEngine &engine, int width, int height, const char *srWinName)
    {
        engine.window.width = width;
        engine.window.height = height;
        engine.window.srWinName = srWinName;

        pWidth = &engine.window.width;
        pHeight = &engine.window.height;
        pWindowResized = &engine.window.bWasResized;

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        engine.window.pWindow = glfwCreateWindow(width, height, srWinName, nullptr, nullptr);
        
        glfwSetFramebufferSizeCallback(engine.window.pWindow, &FramebufferResizeCallback);
        glfwSetKeyCallback(engine.window.pWindow, &KeyBoardInputCallback);
        glfwSetCursorPosCallback(engine.window.pWindow, &MousePositionInputCallback);
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
}