#include "ShadowMappingStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/renderpass/subpasses/ShadowPass.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;

CShadowMappingStage::~CShadowMappingStage()
{
    cleanup();
}

void CShadowMappingStage::create()
{
    detectExtent = true;
    screenExtent = CDevice::inst()->getExtent(detectExtent);

    outReferences.emplace(0, std::vector<vk::AttachmentReference>{});
    depthReference = vk::AttachmentReference{0, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    pRenderPass = Render::CRenderPass::Builder().
    addAttachmentDescription(CImage::getDepthFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal).
    addSubpassDescription(vk::PipelineBindPoint::eGraphics, outReferences[0], &depthReference).
    addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eMemoryRead, vk::AccessFlagBits::eColorAttachmentWrite).
    addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead).
    build();

    pRenderPass->pushSubpass(std::make_shared<CShadowPass>());

    pFramebuffer = std::make_unique<CFramebuffer>();
    pFramebuffer->addImage("shadowmap", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled);

    pRenderPass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pFramebuffer->create(pRenderPass->get(), screenExtent);
    pRenderPass->create();
}

void CShadowMappingStage::rebuild()
{
    screenExtent = CDevice::inst()->getExtent(detectExtent);
    pRenderPass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pRenderPass->reCreate();
    pFramebuffer->reCreate(pRenderPass->get(), screenExtent);
}