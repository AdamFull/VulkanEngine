#pragma once
#include "VulkanMainStructures.h"

namespace Engine::Window
{
    void Initialize(Main::FVulkanRenderInstance &renderInstance, int width, int height, const char *srWinName);
    void Destroy(Main::FVulkanRenderInstance &renderInstance);

    void CreateWindowSurface(Main::FVulkanRenderInstance &renderInstance);

    void PollEvents();
    bool ShouldClose(Main::FVulkanRenderInstance &renderInstance);
    bool WasResized(Main::FVulkanRenderInstance &renderInstance);

    void FramebufferResizeCallback(GLFWwindow *window, int width, int height);
    void KeyBoardInputCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void MousePositionInputCallback(GLFWwindow *window, double xpos, double ypos);

    void ResizeWindow(Main::FVulkanRenderInstance &renderInstance, int width, int height);
}