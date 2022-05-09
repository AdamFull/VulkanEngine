#pragma once
#include "WindowBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Editor
        {
            class CDebugWindow : public CWindowBase
            {
            public:
                CDebugWindow() = default;
                void draw() override;

            private:
                bool show_demo_window = true;
                std::array<float, 50> frameTimes{};
                float frameTimeMin = 0.0f, frameTimeMax = 1.5f;
            };
        }
    }
}