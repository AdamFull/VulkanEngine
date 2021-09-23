#pragma once

namespace Engine
{
    namespace Window
    {
        struct FWindowHandle
        {
            int width, height;
            bool bWasResized{false};
            std::string srWinName;
            GLFWwindow* pWindow;
        };
    }
}