#include "PresentFinalStage.h"
#include "graphics/renderpass/subpasses/ThroughPass.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;

CPresentFinalStage::~CPresentFinalStage()
{
    cleanup();
}

void CPresentFinalStage::create()
{
    screenExtent = CSwapChain::getInstance()->getExtent();
    
    std::vector<vk::AttachmentReference> vReferences_0
    {
        {0, vk::ImageLayout::eColorAttachmentOptimal}
    };
    vk::AttachmentReference depthReference{1, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    pRenderPass = Render::CRenderPass::Builder().
    addAttachmentDescription(CSwapChain::getInstance()->getImageFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR).
    addAttachmentDescription(CImage::getDepthFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal). //Depth stencil
    addSubpassDescription(vk::PipelineBindPoint::eGraphics, vReferences_0, &depthReference).
    addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eMemoryRead, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite).
    addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe,
    vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eMemoryRead).
    build();
    
    pRenderPass->pushSubpass(std::make_shared<CThroughPass>());

    pFramebuffer = std::make_unique<CFramebuffer>();
    pFramebuffer->addImage("present_khr", CSwapChain::getInstance()->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment);
    pFramebuffer->addImage("depth_image", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment);

    pRenderPass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pFramebuffer->create(pRenderPass->get(), screenExtent);
    pRenderPass->create();
}

void CPresentFinalStage::reCreate()
{
    screenExtent = CSwapChain::getInstance()->getExtent();
    pRenderPass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pRenderPass->reCreate();
    pFramebuffer->reCreate(pRenderPass->get());
}