#include "DeferredStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/renderpass/subpasses/GBufferPass.h"
#include "graphics/renderpass/subpasses/PBRCompositionPass.h"

using namespace engine::core;
using namespace engine::core::render;

CDeferredStage::~CDeferredStage()
{
    cleanup();
}

void CDeferredStage::create()
{
    detectExtent = true;
    screenExtent = CDevice::inst()->getExtent(detectExtent);

    auto framebuffer_1 = make_scope<CFramebufferNew>();
    framebuffer_1->setFlipViewport(VK_TRUE);
    framebuffer_1->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    framebuffer_1->addOutputReference(0U, 1U, 2U); //framebuffer_1->addOutputReference(0U, 1U, 2U, 3U);
    framebuffer_1->addOutputReference(1U, 0U);
    framebuffer_1->addInputReference(1U, 1U, 2U, 3U); //framebuffer_1->addInputReference(1U, 1U, 2U, 3U, 4U);
    framebuffer_1->addImage("composition_tex", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    framebuffer_1->addImage("packed_tex", vk::Format::eR32G32B32A32Uint, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    framebuffer_1->addImage("emission_tex", vk::Format::eB10G11R11UfloatPack32, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    //framebuffer_1->addImage("position_tex", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    framebuffer_1->addImage("depth_tex", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    framebuffer_1->addDescription(0U, true);
    framebuffer_1->addDescription(1U);
    framebuffer_1->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader,
    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    framebuffer_1->addSubpassDependency(0, 1, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eColorAttachmentWrite);
    framebuffer_1->addSubpassDependency(1, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eShaderRead);

    framebuffer_1->addRenderer(make_scope<CGBufferPass>());
    framebuffer_1->addRenderer(make_scope<CPBRCompositionPass>());
    vFramebuffer.emplace_back(std::move(framebuffer_1));
    CRenderStage::create();
}

void CDeferredStage::rebuild()
{
    screenExtent = CDevice::inst()->getExtent(detectExtent);
    framebufferIndex = 0;
    for(auto& framebuffer : vFramebuffer)
    {
        framebuffer->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
        framebuffer->reCreate();
        framebufferIndex++;
    }
}