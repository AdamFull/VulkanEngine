#include "WinCallbacks.h"

namespace Engine
{
    std::vector<EasyDelegate::TDelegate<void(int)>> WinCallbacks::focusChangeCallbacks;
    std::vector<EasyDelegate::TDelegate<void(int)>> WinCallbacks::cursorEnterCallbacks;
    std::vector<EasyDelegate::TDelegate<void(int, int, int)>> WinCallbacks::mouseButtonCallbacks;
    std::vector<EasyDelegate::TDelegate<void(float, float)>> WinCallbacks::mousePositionCallbacks;
    std::vector<EasyDelegate::TDelegate<void(float, float)>> WinCallbacks::scrollCallbacks;
    std::vector<EasyDelegate::TDelegate<void(int, int, int, int)>> WinCallbacks::keyCallbacks;
    std::vector<EasyDelegate::TDelegate<void(unsigned int)>> WinCallbacks::charCallbacks;
    std::vector<EasyDelegate::TDelegate<void(int)>> WinCallbacks::monitorCallbacks;

    void WinCallbacks::WinInputFocusChangeCallback(GLFWwindow *window, int focused)
    {
        for(auto& callback : focusChangeCallbacks)
            callback(focused);
    }

    void WinCallbacks::WinInputCursorEnterCallback(GLFWwindow *window, int entered)
    {
        for(auto& callback : cursorEnterCallbacks)
            callback(entered);
    }

    void WinCallbacks::WinInputMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        for(auto& callback : mouseButtonCallbacks)
            callback(button, action, mods);
    }

    void WinCallbacks::WinInputCursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
    {
        for(auto& callback : mousePositionCallbacks)
            callback((float)xpos, (float)ypos);
    }

    void WinCallbacks::WinInputScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
    {
        for(auto& callback : scrollCallbacks)
            callback((float)xoffset, (float)yoffset);
    }

    void WinCallbacks::WinInputKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        for(auto& callback : keyCallbacks)
            callback(key, scancode, action, mods);
    }

    void WinCallbacks::WinInputCharCallback(GLFWwindow *window, unsigned int c)
    {
        for(auto& callback : charCallbacks)
            callback(c);
    }

    void WinCallbacks::WinInputMonitorCallback(GLFWmonitor *window, int monitor)
    {
        for(auto& callback : monitorCallbacks)
            callback(monitor);
    }
}