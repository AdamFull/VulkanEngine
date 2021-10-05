#include "OverlayPropertyEditor.h"

namespace Engine
{
    void OverlayPropertyEditor::Draw()
    {
        if (!ImGui::Begin(srOverlayName.c_str(), &bOverlayState))
        {
            ImGui::End();
            return;
        }

        ImGui::End();
    }

}