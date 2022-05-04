#include "OverlayPropertyEditor.h"

using namespace Engine::Core::Overlay;

void COverlayPropertyEditor::draw()
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