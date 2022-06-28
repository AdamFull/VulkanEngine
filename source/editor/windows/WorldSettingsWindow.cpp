#include "WorldSettingsWindow.h"
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

        ImGui::Separator();
        ImGui::Text("Debug");
        ImGui::Separator();

        ImGui::BeginGroup();
        FControls::DragFloat("bias const", &GlobalVariables::depthBiasConstant, 0.001, 0.0, 5.0);
        FControls::DragFloat("bias slope", &GlobalVariables::depthBiasSlope, 0.001, 0.0, 5.0);
        ImGui::EndGroup();

        ImGui::BeginGroup();
        ImGui::SliderInt("Shading", &GlobalVariables::shadingMode, 0, 2);
        ImGui::SliderInt("Target", &GlobalVariables::debugTarget, 0, 10);
        ImGui::SliderInt("Cascade", &GlobalVariables::debugCascade, 0, 5);
        FControls::DragFloat("Split", &GlobalVariables::cascadeSplitLambda, 0.001, 0.0, 5.0);
        ImGui::EndGroup();

        ImGui::End();
    }
}