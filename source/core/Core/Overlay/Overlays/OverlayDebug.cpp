#include "OverlayDebug.h"
#include <Core/Window/WindowHandle.h>
#include <imgui.h>
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
        auto camera = Core::Scene::Objects::Components::CameraManager::getInstance()->GetCurrentCamera();

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

        glm::vec3 rotation = camera->GetRotation();
        ImGui::Text("Camera");
        ImGui::InputFloat3("Position", glm::value_ptr(camera->viewPos));
        ImGui::InputFloat3("Rotation", glm::value_ptr(rotation));

        ImGui::Text("HDR");
        ImGui::DragFloat("gamma", &GlobalVariables::postprocessGamma, 0.001, 0.0, 5.0);
        ImGui::DragFloat("exposure", &GlobalVariables::postprocessExposure, 0.001, 0.1, 20.0);

        ImGui::Text("PostProcess");
        ImGui::DragFloat("bloom_threshold", &GlobalVariables::bloomThreshold, 0.001, 0.65, 1.0);
        ImGui::DragFloat("blur_scale", &GlobalVariables::blurScale, 0.001, 0.001, 1.0);
        ImGui::DragFloat("blur_strength", &GlobalVariables::blurStrength, 0.001, 0.001, 1.0);
        ImGui::DragInt("blur_direction", &GlobalVariables::direction, 1, 0, 1);

        ImGui::Text("VBO");
        int vertices = UVBO->GetLastVertex();
        ImGui::InputInt("Vertices", &vertices);
        int indices = UVBO->GetLastIndex();
        ImGui::InputInt("Indices", &indices);

        ImGui::End();
    }
}