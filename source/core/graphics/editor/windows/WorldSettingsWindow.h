#pragma once
#include "WindowBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Editor
        {
            class CWorldSettingsWindow : public CWindowBase
            {
            public:
                CWorldSettingsWindow() = default;
                void draw() override;
            };
        }
    }
}