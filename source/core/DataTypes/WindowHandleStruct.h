#pragma once

namespace Engine::Window
{
    struct FWindowHandle
    {
        EasyDelegate::TDelegate<void(int, int, int, int)> keyCodeCallback;
        EasyDelegate::TDelegate<void(double, double)> kousePositionCallback;

        int width, height;
        bool bWasResized{false};
        std::string srWinName;
        GLFWwindow* pWindow;
    };
}