#pragma once
#include <imgui.h>

namespace Engine
{
    class OverlayBase
    {
    public:
        virtual ~OverlayBase() {}
        OverlayBase() = default;
        OverlayBase(std::string)
        {}

        virtual void SetState(bool bState) { bOverlayState = bState; }
        virtual void Draw();
    protected:
        std::string srOverlayName;
        bool bOverlayState{true};
    };
}