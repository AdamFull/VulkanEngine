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

    pFramebuffer = std::make_unique<CFramebufferNew>();
    pFramebuffer->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pFramebuffer->addOutputReference(0, 0);
    pFramebuffer->addImage("present_khr", CDevice::inst()->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment);
    pFramebuffer->addDescription(0);
    pFramebuffer->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);
    
    pFramebuffer->pushSubpass(std::make_shared<CThroughPass>());
    pFramebuffer->create();
}