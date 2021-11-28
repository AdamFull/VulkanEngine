#include "OverlayDebug.h"
#include <Core/Window/WindowHandle.h>
#include <imgui.h>
#include <Core/Scene/Objects/Components/Camera/CameraComponent.h>
#include <Core/Scene/Objects/Components/Camera/CameraManager.h>
#include <GlobalVariables.h>

#include "Core/VulkanVBO.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core::Overlay;

void OverlayDebug::Draw()
{
    if (bOverlayState)
    {
        if (!ImGui::Begin(srOverlayName.c_str(), &bOverlayState))
        {
            ImGui::End();
            return;
        }

        float fFrameTime = 1000.0f / ImGui::GetIO().Framerate;
        auto camera = Core::Scene::Objects::Components::CameraManager::GetInstance()->GetCurrentCamera();
        auto pos = camera->GetPosition();
        float position[3] = {pos.x, pos.y, pos.z};
        auto rot = camera->GetRotation();
        float rotation[3] = {rot.x, rot.y, rot.z};

        int viewport_size[2] = {Window::WindowHandle::m_iWidth, Window::WindowHandle::m_iHeight};

        std::rotate(frameTimes.begin(), frameTimes.begin() + 1, frameTimes.end());
        frameTimes.back() = fFrameTime;

        if (fFrameTime < frameTimeMin)
            frameTimeMin = fFrameTime;
        if (fFrameTime > frameTimeMax)
            frameTimeMax = fFrameTime;

        ImGui::Text("Frame info");
        char overlay[32];
        sprintf(overlay, "dt: %.3f | fps: %.3f", fFrameTime, ImGui::GetIO().Framerate);
        ImGui::PlotLines("", &frameTimes[0], 50, 0, overlay, frameTimeMin, frameTimeMax, ImVec2(0, 80));

        ImGui::InputInt2("Viewport size", viewport_size);

        ImGui::Text("Camera");
        ImGui::InputFloat3("Position", (float *)position);
        ImGui::InputFloat3("Rotation", (float *)rotation);

        ImGui::Text("Scene");
        ImGui::DragFloat("gamma", &GlobalVariables::postprocessGamma, 0.001, 0.0, 5.0);
        ImGui::DragFloat("exposure", &GlobalVariables::postprocessExposure, 0.001, 0.1, 20.0);

        ImGui::Text("VBO");
        int vertices = UVBO->GetLastVertex();
        ImGui::InputInt("Vertices", &vertices);
        int indices = UVBO->GetLastIndex();
        ImGui::InputInt("Indices", &indices);

        ImGui::End();
    }
}