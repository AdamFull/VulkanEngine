#pragma once

namespace Engine
{
    enum class EWinCallbackType
    {
        eFocusChange,
        eCursorEnter,
        eMouseButton,
        eScroll,
        eInputKey,
        eInputChar,
        eMonitor
    };

    struct WinCallbacks
    {
        static void WinInputFocusChangeCallback(GLFWwindow *window, int focused);
        static void WinInputCursorEnterCallback(GLFWwindow *window, int entered);
        static void WinInputMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        static void WinInputScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
        static void WinInputKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void WinInputCharCallback(GLFWwindow *window, unsigned int c);
        static void WinInputMonitorCallback(GLFWmonitor *window, int monitor);

        template<class ...Args>
        static void Subscribe(EWinCallbackType eCallbackType, Args ...args)
        {
            switch (eCallbackType)
            {
            case EWinCallbackType::eFocusChange: { focusChangeCallbacks.emplace_back(EasyDelegate::TDelegate<void(int)>(std::forward<Args>(args)...)); }break;
            case EWinCallbackType::eCursorEnter: { cursorEnterCallbacks.emplace_back(EasyDelegate::TDelegate<void(int)>(std::forward<Args>(args)...)); }break;
            case EWinCallbackType::eMouseButton: { mouseButtonCallbacks.emplace_back(EasyDelegate::TDelegate<void(int, int, int)>(std::forward<Args>(args)...)); }break;
            case EWinCallbackType::eScroll: { scrollCallbacks.emplace_back(EasyDelegate::TDelegate<void(float, float)>(std::forward<Args>(args)...)); }break;
            case EWinCallbackType::eInputKey: { keyCallbacks.emplace_back(EasyDelegate::TDelegate<void(int, int, int, int)>(std::forward<Args>(args)...)); }break;
            case EWinCallbackType::eInputChar: { charCallbacks.emplace_back(EasyDelegate::TDelegate<void(unsigned int)>(std::forward<Args>(args)...)); }break;
            case EWinCallbackType::eMonitor: { monitorCallbacks.emplace_back(EasyDelegate::TDelegate<void(int)>(std::forward<Args>(args)...)); }break;
            }
        }

    private:
        static std::vector<EasyDelegate::TDelegate<void(int)>> focusChangeCallbacks;
        static std::vector<EasyDelegate::TDelegate<void(int)>> cursorEnterCallbacks;
        static std::vector<EasyDelegate::TDelegate<void(int, int, int)>> mouseButtonCallbacks;
        static std::vector<EasyDelegate::TDelegate<void(float, float)>> scrollCallbacks;
        static std::vector<EasyDelegate::TDelegate<void(int, int, int, int)>> keyCallbacks;
        static std::vector<EasyDelegate::TDelegate<void(unsigned int)>> charCallbacks;
        static std::vector<EasyDelegate::TDelegate<void(int)>> monitorCallbacks;
    };
}