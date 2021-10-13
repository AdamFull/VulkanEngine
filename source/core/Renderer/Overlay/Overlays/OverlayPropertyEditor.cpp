#include "OverlayPropertyEditor.h"

namespace Engine
{
    void OverlayPropertyEditor::Draw()
    {
        if (bOverlayState)
        {
            if (!ImGui::Begin(srOverlayName.c_str(), &bOverlayState))
            {
                ImGui::End();
                return;
            }

            ImGui::End();
        }
    }

}