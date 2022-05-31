#include "PresentFinalStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/renderpass/subpasses/ThroughPass.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;

CPresentFinalStage::~CPresentFinalStage()
{
    cleanup();
}

void CPresentFinalStage::create()
{
    screenExtent = CDevice::inst()->getExtent();

    auto framebuffer_1 = make_scope<CFramebufferNew>();
    framebuffer_1->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    framebuffer_1->addOutputReference(0U, 0U);
    framebuffer_1->addImage("present_khr", CDevice::inst()->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment);
    framebuffer_1->addDescription(0U);
    framebuffer_1->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);
    
    framebuffer_1->pushSubpass(make_scope<CThroughPass>());
    vFramebuffer.emplace_back(std::move(framebuffer_1));
    CRenderStage::create();
}