#pragma once
#include "OverlayBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Overlay
        {
            class COverlayPropertyEditor : public COverlayBase
            {
            public:
                COverlayPropertyEditor() = default;
                COverlayPropertyEditor(std::string srName)
                {
                    srOverlayName = srName;
                    bOverlayState = true;
                }
                void draw() override;
            };
        }
    }
}