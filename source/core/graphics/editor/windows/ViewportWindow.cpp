#include "ViewportWindow.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "graphics/editor/imgui_impl_vulkan.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/descriptor/DescriptorSet.h"
#include "Resources/ResourceManager.h"
#include <graphics/scene/objects/components/camera/CameraManager.h>
#include "graphics/editor/Editor.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Editor;
using namespace Engine::Resources;

static const float identityMatrix[16] =
    { 1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f };

ImGuizmo::OPERATION mCurrentGizmoOperation{};

void CViewportWindow::create()
{
    auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
    pDescriptorSet = std::make_shared<CDescriptorSet>();
    pDescriptorSet->create(vk::PipelineBindPoint::eGraphics, pBackend->PipelineLayout, CEditorUI::inst()->getDescriptorPool(), pBackend->DescriptorSetLayout, CDevice::inst()->getFramesInFlight());
}

void CViewportWindow::reCreate()
{
    auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
    pDescriptorSet->create(vk::PipelineBindPoint::eGraphics, pBackend->PipelineLayout, CEditorUI::inst()->getDescriptorPool(), pBackend->DescriptorSetLayout, CDevice::inst()->getFramesInFlight());
}

void CViewportWindow::draw()
{
    if (bOverlayState)
    {
        //CDevice::inst()->GPUWait();
        auto currentImage = CDevice::inst()->getCurrentFrame();
        if (!ImGui::Begin("Viewport", &bOverlayState))
        {
            ImGui::End();
            return;
        }

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        ImVec2 viewportPanelPos = ImGui::GetWindowPos();
        auto textDrawPos = ImGui::GetCursorStartPos();
        
        ImGui::Image(pDescriptorSet->get(currentImage), ImVec2{viewportPanelSize.x, viewportPanelSize.y});

        //Manipulator
        {
            if (ImGui::IsKeyPressed('T'))
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            if (ImGui::IsKeyPressed('R'))
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
            if (ImGui::IsKeyPressed('E')) // r Key
                mCurrentGizmoOperation = ImGuizmo::SCALE;

            auto selected = CEditor::inst()->getLastSelection();
            if(selected)
            {
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist();
                ImGuizmo::SetRect(viewportPanelPos.x, viewportPanelPos.y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
                auto camera = Core::Scene::CCameraManager::inst()->getCurrentCamera();
                auto view = camera->getGizmoView();
                auto projection = camera->getProjection();
                projection[1][1] *= -1.f;
                auto model = selected->getTransform().getModel();
                ImGuizmo::DrawGrid(glm::value_ptr(view), glm::value_ptr(projection), identityMatrix, 100.f);
                if(ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), mCurrentGizmoOperation, ImGuizmo::WORLD, glm::value_ptr(model)))
                {
                    glm::vec3 scale;
                    glm::quat rot;
                    glm::vec3 pos;
                    glm::vec3 skew;
                    glm::vec4 perspective;
                    glm::decompose(model, scale, rot, pos, skew, perspective);
                    selected->setPosition(pos);
                    //selected->setRotation(rot);
                    selected->setScale(scale);
                }
            }
        }

        ImGui::SetCursorPos(textDrawPos);
        auto& io = ImGui::GetIO();
        char overlay[32];
        float fFrameTime = 1000.0f / io.Framerate;
        sprintf(overlay, "dt: %.3f | FPS: %.3f", fFrameTime, io.Framerate);
        ImGui::Text(overlay);

        auto& physical = CDevice::inst()->getPhysical();
        auto props = physical.getProperties();
        sprintf(overlay, "GPU: %s", props.deviceName.data());
        ImGui::Text(overlay);

        auto imageDescriptor = CRenderSystem::inst()->getPrevStage()->getFramebuffer()->getCurrentImages()["output_color"];
        imageDescriptor->updateDescriptor();
        vk::WriteDescriptorSet write{};
        write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        write.dstBinding = 0;
        write.pImageInfo = &imageDescriptor->getDescriptor();
        write.descriptorCount = 1;
        pDescriptorSet->update(write, currentImage);

        ImGui::End();
    }
}