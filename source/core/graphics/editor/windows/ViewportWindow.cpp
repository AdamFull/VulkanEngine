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

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        auto imageDescriptor = CRenderSystem::inst()->getPrevStage()->getFramebuffer()->getCurrentImages()["output_color"];
        ImGui::Image(imageDescriptor->getDescriptorSet(), ImVec2{viewportPanelSize.x, viewportPanelSize.y});
        //auto loadedImage = CResourceManager::inst()->get<CImage>("floor_mesh_component_texture_vintage-tile1_normal-ogl_0");
        //ImGui::Image(loadedImage->getDescriptorSet(), ImVec2{viewportPanelSize.x, viewportPanelSize.y});
        ImGui::End();
    }
}