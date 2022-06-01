#include "WorldSettingsWindow.h"
#include <glm/gtc/type_ptr.hpp>
#include "editor/CustomControls.h"
#include <GlobalVariables.h>

using namespace engine::editor;

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

        ImGui::BeginGroup();
        FControls::DragFloat("gamma", &GlobalVariables::postprocessGamma, 0.001, 0.0, 5.0);
        FControls::DragFloat("exposure", &GlobalVariables::postprocessExposure, 0.001, 0.1, 20.0);
        ImGui::EndGroup();

        ImGui::Separator();
        ImGui::Text("Bloom");
        ImGui::Separator();

        ImGui::BeginGroup();
        FControls::DragFloat("threshold", &GlobalVariables::bloomThreshold, 0.001, 0.0, 5.0);
        FControls::DragFloat("scale", &GlobalVariables::blurScale, 0.001, 0.0, 5.0);
        FControls::DragFloat("strength", &GlobalVariables::blurStrength, 0.001, 0.0, 5.0);
        ImGui::EndGroup();

        ImGui::End();
    }
}