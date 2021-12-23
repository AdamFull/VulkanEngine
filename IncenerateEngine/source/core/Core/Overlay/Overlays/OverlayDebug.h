#pragma once
#include "OverlayBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Overlay
        {
            class OverlayDebug : public OverlayBase
            {
            public:
                OverlayDebug() = default;
                OverlayDebug(std::string srName)
                {
                    srOverlayName = srName;
                }
                void Draw() override;

            private:
                bool show_demo_window = true;
                std::array<float, 50> frameTimes{};
                float frameTimeMin = 0.0f, frameTimeMax = 1.5f;
            };
        }
    }
}