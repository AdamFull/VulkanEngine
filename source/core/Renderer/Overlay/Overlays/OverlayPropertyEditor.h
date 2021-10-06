#pragma once
#include "OverlayBase.h"

namespace Engine
{
    class OverlayPropertyEditor : public OverlayBase
    {
    public:
        OverlayPropertyEditor() = default;
        OverlayPropertyEditor(std::string srName)
        {
            srOverlayName = srName;
            bOverlayState = false;
        }
        void Draw() override;
    };
}