#include "OverlayDebug.h"
#include <graphics/window/WindowHandle.h>
#include <imgui.h>
#include <graphics/scene/objects/components/camera/CameraManager.h>
#include <GlobalVariables.h>

#include "graphics/buffer/VulkanVBO.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core::Overlay;

void COverlayDebug::draw()
{
    if (bOverlayState)
    {
        if (!ImGui::Begin(srOverlayName.c_str(), &bOverlayState))
        {
            ImGui::End();
            return;
        }

        float fFrameTime = 1000.0f / ImGui::GetIO().Framerate;
        auto camera = Core::Scene::CCameraManager::getInstance()->getCurrentCamera();

        int viewport_size[2] = {Window::CWindowHandle::m_iWidth, Window::CWindowHandle::m_iHeight};

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

        glm::vec3 rotation = camera->getRotation();
        ImGui::Text("Camera");
        ImGui::InputFloat3("Position", glm::value_ptr(camera->viewPos));
        ImGui::InputFloat3("Rotation", glm::value_ptr(rotation));

        ImGui::Text("HDR");
        ImGui::DragFloat("gamma", &GlobalVariables::postprocessGamma, 0.001, 0.0, 5.0);
        ImGui::DragFloat("exposure", &GlobalVariables::postprocessExposure, 0.001, 0.1, 20.0);

        ImGui::Text("PostProcess");
        ImGui::DragFloat("bloom_threshold", &GlobalVariables::bloomThreshold, 0.001, 0.65, 1.0);
        ImGui::DragFloat("blur_scale", &GlobalVariables::blurScale, 0.001, 0.003, 1.0);
        ImGui::DragFloat("blur_strength", &GlobalVariables::blurStrength, 0.001, 0.1, 1.0);

        ImGui::Text("VBO");
        int vertices = CVBO::getInstance()->getLastVertex();
        ImGui::InputInt("Vertices", &vertices);
        int indices = CVBO::getInstance()->getLastIndex();
        ImGui::InputInt("Indices", &indices);

        ImGui::End();
    }
}