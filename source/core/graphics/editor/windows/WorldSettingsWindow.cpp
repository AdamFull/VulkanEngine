#include "WorldSettingsWindow.h"
#include <glm/gtc/type_ptr.hpp>
#include "graphics/editor/CustomControls.h"
#include <GlobalVariables.h>

using namespace Engine::Core::Editor;

void CWorldSettingsWindow::draw()
{
    if (bOverlayState)
    {
        if (!ImGui::Begin("World", &bOverlayState))
        {
            ImGui::End();
            return;
        }

        ImGui::Separator();
        ImGui::Text("HDR");
        ImGui::Separator();

        FControls::DragFloat("gamma", &GlobalVariables::postprocessGamma, 0.001, 0.0, 5.0);
        FControls::DragFloat("exposure", &GlobalVariables::postprocessExposure, 0.001, 0.1, 20.0);

        ImGui::End();
    }
}