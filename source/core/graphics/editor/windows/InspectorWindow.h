#pragma once
#include "WindowBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Editor
        {
            class CInspectorWindow : public CWindowBase
            {
            public:
                CInspectorWindow() = default;
                void draw() override;
            };
        }
    }
}