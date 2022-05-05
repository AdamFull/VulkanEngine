#include "OverlayViewport.h"
#include <imgui.h>
#include "graphics/overlay/imgui_impl_vulkan.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/descriptor/DescriptorSet.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Overlay;

void COverlayViewport::create()
{
    for(size_t i = 0; i < CSwapChain::getInstance()->getFramesInFlight(); i++)
    {
        auto image = CRenderSystem::getInstance()->getPrevStage()->getFramebuffer()->getImages(i)["output_color"]->getDescriptor();
        descriptors.emplace_back(ImGui_ImplVulkan_AddTexture(image.sampler, image.imageView, static_cast<VkImageLayout>(image.imageLayout)));
    }
    auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
    pDescriptorSet = std::make_shared<CDescriptorSet>();
    pDescriptorSet->create(vk::PipelineBindPoint::eGraphics, pBackend->PipelineLayout, CImguiOverlay::getInstance()->getDescriptorPool(), pBackend->DescriptorSetLayout, CSwapChain::getInstance()->getFramesInFlight());
    //pDescriptorPtr = std::make_unique<VkDescriptorSet>();
}

void COverlayViewport::draw()
{
    if (bOverlayState)
    {
        auto currentImage = CSwapChain::getInstance()->getCurrentFrame();
        if (!ImGui::Begin(srOverlayName.c_str(), &bOverlayState))
        {
            ImGui::End();
            return;
        }
        auto extent = CSwapChain::getInstance()->getExtent();
        auto imageDescriptor = CRenderSystem::getInstance()->getPrevStage()->getFramebuffer()->getCurrentImages()["output_color"]->getDescriptor();
        vk::WriteDescriptorSet write{};
        write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        //write.dstBinding = 0;
        write.pImageInfo = &imageDescriptor;
        write.descriptorCount = 1;
        pDescriptorSet->update(write, currentImage);

        //auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        ImGui::Image(descriptors.at(currentImage), ImVec2{viewportPanelSize.x, viewportPanelSize.y});
        ImGui::End();
    }
}