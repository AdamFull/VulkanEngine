#include "OverlayDebug.h"
#include <Renderer/Window/WindowHandle.h>
#include <imgui.h>
#include <Objects/Components/Camera/CameraComponent.h>
#include <Objects/Components/Camera/CameraManager.h>
#include <GlobalVariables.h>

namespace Engine
{
    void OverlayDebug::Draw()
    {
        if(bOverlayState)
        {
            if(!ImGui::Begin(srOverlayName.c_str(), &bOverlayState))
            {
                ImGui::End();
                return;
            }

            float fFrameTime = 1000.0f / ImGui::GetIO().Framerate;
            auto camera = CameraManager::GetInstance()->GetCurrentCamera();
            auto pos = camera->GetPosition();
            float position[3] = {pos.x, pos.y, pos.z};
            auto rot = camera->GetRotation();
            float rotation[3] = {rot.x, rot.y, rot.z};

            int viewport_size[2] = {WindowHandle::m_iWidth, WindowHandle::m_iHeight};
            
            std::rotate(frameTimes.begin(), frameTimes.begin() + 1, frameTimes.end());
            frameTimes.back() = fFrameTime;

            ImGui::Text("Frame info");
            char overlay[32];
            sprintf(overlay, "dt: %.3f | fps: %.3f", fFrameTime, ImGui::GetIO().Framerate);
            ImGui::PlotLines("", &frameTimes[0], 50, 0, overlay, frameTimeMin, frameTimeMax, ImVec2(0, 80));

            ImGui::InputInt2("Viewport size", viewport_size);

            ImGui::Text("Camera");
            ImGui::InputFloat3("Position", (float*)position);
            ImGui::InputFloat3("Rotation", (float*)rotation);

            ImGui::Text("Light");
            ImGui::DragFloat3("Light pos", (float*)GlobalVariables::lightPosition, 0.01, -100.0, 100.0);
            ImGui::DragFloat3("Light color", (float*)GlobalVariables::lightColor, 0.005, 0.0, 1.0);

            ImGui::Text("Global");
            ImGui::DragFloat3("Models rot", (float*)GlobalVariables::modelRotation, 0.01, -100.0, 100.0);
            //ImGui::InputFloat3("Light pos", (float*)GlobalVariables::lightPosition);

            ImGui::End();
        }
    }
}