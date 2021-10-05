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
        }
        void Draw() override;
    };
}