#include "ViewportWindow.h"
#include <imgui.h>
#include "graphics/editor/imgui_impl_vulkan.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/descriptor/DescriptorSet.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Editor;

void CViewportWindow::create()
{
    //auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
    //pDescriptorSet = std::make_shared<CDescriptorSet>();
    //pDescriptorSet->create(vk::PipelineBindPoint::eGraphics, pBackend->PipelineLayout, CEditorUI::inst()->getDescriptorPool(), pBackend->DescriptorSetLayout, CDevice::inst()->getFramesInFlight());
    //pDescriptorPtr = std::make_unique<VkDescriptorSet>();
}

void CViewportWindow::reCreate()
{
    //auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
    //pDescriptorSet->create(vk::PipelineBindPoint::eGraphics, pBackend->PipelineLayout, CEditorUI::inst()->getDescriptorPool(), pBackend->DescriptorSetLayout, CDevice::inst()->getFramesInFlight());
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
        auto extent = CDevice::inst()->getExtent();
        auto imageDescriptor = CRenderSystem::inst()->getPrevStage()->getFramebuffer()->getCurrentImages()["output_color"];

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        ImGui::Image(imageDescriptor->getDescriptorSet(), ImVec2{viewportPanelSize.x, viewportPanelSize.y});
        ImGui::End();
    }
}