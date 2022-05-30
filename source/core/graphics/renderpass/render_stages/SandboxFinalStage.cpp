#include "SandboxFinalStage.h"
#include "graphics/renderpass/subpasses/ImguiPass.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;

CSandboxFinalStage::~CSandboxFinalStage()
{
    cleanup();
}

void CSandboxFinalStage::create()
{
    screenExtent = CDevice::inst()->getExtent();

    auto framebuffer_1 = std::make_unique<CFramebufferNew>();
    framebuffer_1->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    framebuffer_1->addOutputReference(0U, 0U);
    framebuffer_1->addImage("present_khr", CDevice::inst()->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment);
    framebuffer_1->addDescription(0U);
    framebuffer_1->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);

    framebuffer_1->pushSubpass(std::make_shared<CImguiPass>());
    vFramebuffer.emplace_back(std::move(framebuffer_1));
    CRenderStage::create();
}