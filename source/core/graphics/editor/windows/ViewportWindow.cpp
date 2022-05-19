#include "ViewportWindow.h"
#include <imgui.h>
#include <imgui_internal.h>
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

ImGuizmo::OPERATION mCurrentGizmoOperation{ImGuizmo::TRANSLATE};

void CViewportWindow::create()
{
    auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
    pDescriptorSet = std::make_shared<CDescriptorSet>();
    pDescriptorSet->create(vk::PipelineBindPoint::eGraphics, pBackend->PipelineLayout, CEditorUI::inst()->getDescriptorPool(), pBackend->DescriptorSetLayout);
}

void CViewportWindow::reCreate()
{
    auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
    pDescriptorSet->create(vk::PipelineBindPoint::eGraphics, pBackend->PipelineLayout, CEditorUI::inst()->getDescriptorPool(), pBackend->DescriptorSetLayout);
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
    auto currentImage = CDevice::inst()->getCurrentFrame();
    auto imageDescriptor = CRenderSystem::inst()->getPrevStage()->getFramebuffer()->getCurrentImages()["output_color"];
    imageDescriptor->updateDescriptor();
    vk::WriteDescriptorSet write{};
    write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    write.dstBinding = 0;
    write.pImageInfo = &imageDescriptor->getDescriptor();
    write.descriptorCount = 1;
    pDescriptorSet->update(write);

    ImGui::Image(pDescriptorSet->get(), ImVec2{offsetx, offsety});
    
    if(!ImGui::IsAnyItemActive())
        CDevice::inst()->setViewportExtent(vk::Extent2D{(uint32_t)offsetx, (uint32_t)offsety});
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

    auto selected = CEditor::inst()->getLastSelection();
    if(selected)
    {
        auto camera = Core::Scene::CCameraManager::inst()->getCurrentCamera();
        camera->setControl(ImGui::IsWindowHovered(ImGuiFocusedFlags_RootAndChildWindows));

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(offsetx, offsety, sizex, sizey);

        //Camera view
        auto view = camera->getView();
        //Camera projection
        auto projection = camera->getProjection();
        //projection = glm::scale(projection, glm::vec3(1.0, -1.0, 1.0));
        //Object transform
        auto model = selected->getTransform().getModel();

        //Manipulating
        if(ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), mCurrentGizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(model), nullptr, snap ? snapValues : nullptr))
        {
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(model, scale, rotation, translation, skew,perspective);

            switch (mCurrentGizmoOperation)
            {
            case ImGuizmo::TRANSLATE: selected->setPosition(translation); break;
            case ImGuizmo::ROTATE: selected->setRotation(glm::eulerAngles(glm::conjugate(rotation))); break;
            case ImGuizmo::SCALE: selected->setScale(scale); break;
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

    auto& physical = CDevice::inst()->getPhysical();
    auto props = physical.getProperties();
    sprintf(overlay, "GPU: %s", props.deviceName.data());
    ImGui::Text(overlay);

    sprintf(overlay, "Vertices: %zu, Indices %zu", CVBO::inst()->getLastVertex(), CVBO::inst()->getLastIndex());
    ImGui::Text(overlay);

    //overlayX = ImGui::GetCursorPosX();
    overlayY = ImGui::GetCursorPosY();

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}