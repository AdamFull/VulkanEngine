#pragma once
#include "OverlayBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Overlay
        {
            class COverlayLog : public COverlayBase
            {
            public:
                COverlayLog() = default;
                COverlayLog(std::string srName)
                {
                    srOverlayName = srName;
                    bOverlayState = false;
                }
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