#pragma once
#include <imgui.h>

namespace Engine
{
    namespace Core
    {
        namespace Overlay
        {
            class COverlayBase
            {
            public:
                virtual ~COverlayBase() {}
                COverlayBase() = default;
                COverlayBase(std::string)
                {
                }

                virtual void create() {}
                virtual void reCreate() {}
                virtual void setState(bool bState) { bOverlayState = bState; }
                virtual void draw();

            protected:
                std::string srOverlayName;
                bool bOverlayState{true};
            };
        }
    }
}