#pragma once
#include "util/function.hpp"

namespace Engine
{
    namespace Core
    {
        namespace Window
        {
            enum class EWinCallbackType
            {
                eFocusChange,
                eCursorEnter,
                eMouseButton,
                eMousePosition,
                eScroll,
                eInputKey,
                eInputChar,
                eMonitor
            };

            struct WinCallbacks
            {
                static void WinSizeChangeCallback(GLFWwindow *window, int width, int height);
                static void WinInputFocusChangeCallback(GLFWwindow *window, int focused);
                static void WinInputCursorEnterCallback(GLFWwindow *window, int entered);
                static void WinInputMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
                static void WinInputCursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
                static void WinInputScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
                static void WinInputKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
                static void WinInputCharCallback(GLFWwindow *window, unsigned int c);
                static void WinInputMonitorCallback(GLFWmonitor *window, int monitor);

                template <class... Args>
                static void SubscribeSizeChange(Args... args)
                {
                    sizeChangeCallbacks.emplace_back(utl::function<void(int, int)>(std::forward<Args>(args)...));
                }

                template <class... Args>
                static void SubscribeFocusChange(Args... args)
                {
                    focusChangeCallbacks.emplace_back(utl::function<void(int)>(std::forward<Args>(args)...));
                }

                template <class... Args>
                static void SubscribeCursorEnter(Args... args)
                {
                    cursorEnterCallbacks.emplace_back(utl::function<void(int)>(std::forward<Args>(args)...));
                }

                template <class... Args>
                static void SubscribeMouseButton(Args... args)
                {
                    mouseButtonCallbacks.emplace_back(utl::function<void(int, int, int)>(std::forward<Args>(args)...));
                }

                template <class... Args>
                static void SubscribeMousePosition(Args... args)
                {
                    mousePositionCallbacks.emplace_back(utl::function<void(float, float)>(std::forward<Args>(args)...));
                }

                template <class... Args>
                static void SubscribeMouseScroll(Args... args)
                {
                    scrollCallbacks.emplace_back(utl::function<void(float, float)>(std::forward<Args>(args)...));
                }

                template <class... Args>
                static void SubscribeKeyInput(Args... args)
                {
                    keyCallbacks.emplace_back(utl::function<void(int, int, int, int)>(std::forward<Args>(args)...));
                }

                template <class... Args>
                static void SubscribeCharInput(Args... args)
                {
                    charCallbacks.emplace_back(utl::function<void(unsigned int)>(std::forward<Args>(args)...));
                }

                template <class... Args>
                static void SubscribeMonitor(Args... args)
                {
                    monitorCallbacks.emplace_back(utl::function<void(int)>(std::forward<Args>(args)...));
                }

            private:
                static std::vector<utl::function<void(int, int)>> sizeChangeCallbacks;
                static std::vector<utl::function<void(int)>> focusChangeCallbacks;
                static std::vector<utl::function<void(int)>> cursorEnterCallbacks;
                static std::vector<utl::function<void(int, int, int)>> mouseButtonCallbacks;
                static std::vector<utl::function<void(float, float)>> mousePositionCallbacks;
                static std::vector<utl::function<void(float, float)>> scrollCallbacks;
                static std::vector<utl::function<void(int, int, int, int)>> keyCallbacks;
                static std::vector<utl::function<void(unsigned int)>> charCallbacks;
                static std::vector<utl::function<void(int)>> monitorCallbacks;
            };
        }
    }
}