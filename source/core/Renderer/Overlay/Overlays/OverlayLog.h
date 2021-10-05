#pragma once
#include "OverlayBase.h"

namespace Engine
{
    class OverlayLog : public OverlayBase
    {
    public:
        OverlayLog() = default;
        OverlayLog(std::string srName)
        {
            srOverlayName = srName;
        }
        void Draw() override;
        
    private:
        void Clear();

        ImGuiTextBuffer     Buf;
        ImGuiTextFilter     Filter;
        ImVector<int>       LineOffsets;
        bool                AutoScroll; 
    };
}