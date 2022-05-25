#include "DeferredStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/renderpass/subpasses/GBufferPass.h"
#include "graphics/renderpass/subpasses/PBRCompositionPass.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;

CDeferredStage::~CDeferredStage()
{
    cleanup();
}

void CDeferredStage::create()
{
    detectExtent = true;
    screenExtent = CDevice::inst()->getExtent(detectExtent);

    pFramebuffer = std::make_unique<CFramebufferNew>();
    pFramebuffer->setFlipViewport(VK_TRUE);
    pFramebuffer->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    pFramebuffer->addOutputReference(0, 1, 2);
    pFramebuffer->addOutputReference(1, 0, 3);
    pFramebuffer->addInputReference(1, 1, 2, 4);
    pFramebuffer->addImage("output_color", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    pFramebuffer->addImage("packed_tex", vk::Format::eR32G32B32A32Uint, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("emission_tex", vk::Format::eR8G8B8A8Snorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("brightness_buffer", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    pFramebuffer->addImage("depth_image", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addDescription(0, true);
    pFramebuffer->addDescription(1);
    pFramebuffer->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader,
    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    pFramebuffer->addSubpassDependency(0, 1, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eColorAttachmentWrite);
    pFramebuffer->addSubpassDependency(1, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eShaderRead);

    pFramebuffer->pushSubpass(std::make_shared<CGBufferPass>());
    pFramebuffer->pushSubpass(std::make_shared<CPBRCompositionPass>());
    pFramebuffer->create();
}

void CDeferredStage::rebuild()
{
    screenExtent = CDevice::inst()->getExtent(detectExtent);
    pFramebuffer->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pFramebuffer->reCreate();
}