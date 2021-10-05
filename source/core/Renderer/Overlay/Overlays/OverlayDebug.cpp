#include "OverlayDebug.h"
#include <Renderer/Window/WindowHandle.h>
#include <imgui.h>
#include <Camera/Camera.h>
#include <Camera/CameraManager.h>

namespace Engine
{
    void OverlayDebug::Draw()
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

        ImGui::End();
    }
}