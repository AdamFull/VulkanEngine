#pragma once
#include "VulkanMainStructures.h"

namespace Engine::Window
{
    EasyDelegate::TDelegate<void(int, int, int, int)> KeyCodeCallback;
    EasyDelegate::TDelegate<void(double, double)> MousePositionCallback;

    int* pWidth{nullptr};
    int* pHeight{nullptr};
    bool* pWindowResized{nullptr};

    void Initialize(Main::FVulkanEngine &engine, int width, int height, const char *srWinName);
    void Destroy(Main::FVulkanEngine &engine);

    void CreateWindowSurface(Main::FVulkanEngine &engine);

    void PollEvents();
    bool ShouldClose(Main::FVulkanEngine &engine);

    void FramebufferResizeCallback(GLFWwindow *window, int width, int height);
    void KeyBoardInputCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void MousePositionInputCallback(GLFWwindow *window, double xpos, double ypos);
}