#include "PostProcessStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/renderpass/subpasses/DownsamplePass.h"
#include "graphics/renderpass/subpasses/BlurPass.h"
#include "graphics/renderpass/subpasses/FinalCompositionPass.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;

CPostProcessStage::~CPostProcessStage()
{
    cleanup();
}

void CPostProcessStage::create()
{
    detectExtent = true;
    screenExtent = CDevice::inst()->getExtent(detectExtent);

    pFramebuffer = std::make_unique<CFramebufferNew>();
    pFramebuffer->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pFramebuffer->addOutputReference(0, 0);
    pFramebuffer->addImage("output_color", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
    pFramebuffer->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);
    pFramebuffer->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead);

    pFramebuffer->pushSubpass(std::make_shared<CFinalCompositionPass>());
    pFramebuffer->create();
}

void CPostProcessStage::render(vk::CommandBuffer& commandBuffer)
{
    pFramebuffer->render(commandBuffer);
}

//TODO: make it better
void CPostProcessStage::rebuild()
{
    screenExtent = CDevice::inst()->getExtent(detectExtent);
    pFramebuffer->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pFramebuffer->reCreate();
}