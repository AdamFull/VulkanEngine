#pragma once
#include <imgui.h>

namespace Engine
{
    namespace Core
    {
        namespace Editor
        {
            class CWindowBase
            {
            public:
                virtual ~CWindowBase() {}
                CWindowBase() = default;

                virtual void create() {}
                virtual void reCreate() {}
                virtual void setState(bool bState) { bOverlayState = bState; }
                virtual void draw();

            protected:
                bool bOverlayState{true};
            };
        }
    }
}