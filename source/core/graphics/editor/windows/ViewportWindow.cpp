#include "ViewportWindow.h"
#include <imgui.h>
#include "graphics/editor/imgui_impl_vulkan.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/descriptor/DescriptorSet.h"
#include "Resources/ResourceManager.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Editor;
using namespace Engine::Resources;

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
        auto currentImage = CDevice::inst()->getCurrentFrame();
        if (!ImGui::Begin("Viewport", &bOverlayState))
        {
            ImGui::End();
            return;
        }

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        auto textDrawPos = ImGui::GetCursorStartPos();
        
        ImGui::Image(pDescriptorSet->get(currentImage), ImVec2{viewportPanelSize.x, viewportPanelSize.y});

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