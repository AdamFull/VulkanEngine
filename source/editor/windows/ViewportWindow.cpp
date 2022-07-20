#include "ViewportWindow.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>
#include "graphics/VulkanHighLevel.h"
#include "editor/imgui_impl_vulkan.h"
#include "graphics/VulkanHighLevel.h"
#include "Resources/ResourceManager.h"
#include "graphics/scene/objects/components/CameraManager.h"

using namespace engine::editor;
using namespace engine::core;
using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::core::descriptor;
using namespace engine::resources;

static const float identityMatrix[16] =
    { 1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f };

ImGuizmo::OPERATION mCurrentGizmoOperation{ImGuizmo::TRANSLATE};

void CViewportWindow::create()
{
    auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
    pDescriptorSet = utl::make_scope<CDescriptorSet>(vk::PipelineBindPoint::eGraphics, pBackend->PipelineLayout, UEditorUI->getDescriptorPool(), pBackend->DescriptorSetLayout);
}

void CViewportWindow::reCreate()
{
    auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
    pDescriptorSet = utl::make_scope<CDescriptorSet>(vk::PipelineBindPoint::eGraphics, pBackend->PipelineLayout, UEditorUI->getDescriptorPool(), pBackend->DescriptorSetLayout);
}

void CViewportWindow::draw()
{
    if (bOverlayState)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        if (!ImGui::Begin("Viewport", &bOverlayState))
        {
            ImGui::End();
            return;
        }

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        ImVec2 viewportPanelPos = ImGui::GetWindowPos();
        auto textDrawPos = ImGui::GetCursorStartPos();
        
        drawViewport(viewportPanelSize.x, viewportPanelSize.y);
        drawManipulator(viewportPanelPos.x, viewportPanelPos.y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
        drawOverlay(textDrawPos.x, textDrawPos.y);

        ImGui::End();
        ImGui::PopStyleVar();
    }
}

void CViewportWindow::drawViewport(float offsetx, float offsety)
{
    auto currentImage = UDevice->getCurrentFrame();
    auto imageDescriptor = URenderer->getCurrentImages()["output_color"];
    imageDescriptor->updateDescriptor();
    vk::WriteDescriptorSet write{};
    write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    write.dstBinding = 0;
    write.pImageInfo = &imageDescriptor->getDescriptor();
    write.descriptorCount = 1;
    pDescriptorSet->update(write);

    ImGui::Image(pDescriptorSet->get(), ImVec2{offsetx, offsety});
    
    if(!ImGui::IsAnyItemActive())
        UDevice->setViewportExtent(vk::Extent2D{(uint32_t)offsetx, (uint32_t)offsety});
}

void CViewportWindow::drawManipulator(float offsetx, float offsety, float sizex, float sizey)
{
    bool snap{false};
    //Select manipulator mode
    if (ImGui::IsKeyPressed('T'))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed('R'))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed('E'))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if(ImGui::IsKeyPressed('Z'))
        snap = true;

    float snapValue = 0.5f;
    if (mCurrentGizmoOperation == ImGuizmo::OPERATION::ROTATE)
	    snapValue = 45.0f;
    float snapValues[3] = { snapValue, snapValue, snapValue };

    auto selected = UEditor->getLastSelection();
    if(selected)
    {
        auto& cameraNode = UCamera->getCurrentCamera();
        auto& camera = cameraNode->getCamera();
        camera->setControl(ImGui::IsWindowHovered(ImGuiFocusedFlags_RootAndChildWindows));

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(offsetx, offsety, sizex, sizey);

        //Camera view
        auto view = camera->getView();
        //Camera projection
        auto projection = camera->getProjection();
        //Object transform
        auto model = selected->getModel();
        glm::mat4 delta;

        //Manipulating
        if(ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), mCurrentGizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(model), glm::value_ptr(delta), nullptr))
        {
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(delta, scale, rotation, translation, skew,perspective);

            auto& localTransform = selected->getLocalTransform();
            switch (mCurrentGizmoOperation)
            {
            case ImGuizmo::TRANSLATE: localTransform.pos += translation; break;
            case ImGuizmo::ROTATE: localTransform.rot += glm::eulerAngles(glm::conjugate(rotation)); break;
            case ImGuizmo::SCALE: localTransform.scale *= scale; break;
            }
        }
    }
}

void CViewportWindow::drawOverlay(float offsetx, float offsety)
{
    ImGui::SetCursorPos(ImVec2(offsetx, offsety));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, {0.23529413f, 0.24705884f, 0.25490198f, 0.50f});
    ImGui::BeginChild("ChildR", ImVec2(overlayX, overlayY), false, window_flags);

    auto& io = ImGui::GetIO();
    char overlay[64];
    float fFrameTime = 1000.0f / io.Framerate;
    sprintf(overlay, "dt: %.3f | FPS: %.3f", fFrameTime, io.Framerate);
    ImGui::Text(overlay);

    auto& physical = UDevice->getPhysical();
    auto props = physical.getProperties();
    sprintf(overlay, "GPU: %s", props.deviceName.data());
    ImGui::Text(overlay);

    sprintf(overlay, "Vertices: %zu, Indices %zu", UVBO->getLastVertex(), UVBO->getLastIndex());
    ImGui::Text(overlay);

    //overlayX = ImGui::GetCursorPosX();
    overlayY = ImGui::GetCursorPosY();

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}