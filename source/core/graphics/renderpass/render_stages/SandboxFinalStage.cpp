#include "SandboxFinalStage.h"
#include "graphics/renderpass/subpasses/ImguiPass.h"
#include "graphics/VulkanHighLevel.h"

using namespace engine::core;
using namespace engine::core::render;

void CSandboxFinalStage::create()
{
    screenExtent = UDevice->getExtent();

    auto framebuffer_1 = utl::make_scope<CFramebufferNew>();
    framebuffer_1->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    framebuffer_1->addImage("present_khr", UDevice->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment);
    framebuffer_1->addOutputReference(0U, "present_khr");
    framebuffer_1->addDescription(0U);
    framebuffer_1->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);

    framebuffer_1->addRenderer(utl::make_scope<CImguiPass>());
    vFramebuffer.emplace_back(std::move(framebuffer_1));
    CRenderStage::create();
}