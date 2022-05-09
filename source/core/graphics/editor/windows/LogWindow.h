#pragma once
#include "WindowBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Editor
        {
            class CLogWindow : public CWindowBase
            {
            public:
                CLogWindow() = default;
                void draw() override;

            private:
                void clear();

                ImGuiTextBuffer Buf;
                ImGuiTextFilter Filter;
                ImVector<int> LineOffsets;
                bool AutoScroll;
            };
        }
    }
}