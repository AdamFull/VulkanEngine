#include "SandboxFinalStage.h"
#include "graphics/renderpass/subpasses/ImguiPass.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;

CSandboxFinalStage::~CSandboxFinalStage()
{
    cleanup();
}

void CSandboxFinalStage::create()
{
    screenExtent = CDevice::inst()->getExtent();

    outReferences.emplace(0, std::vector<vk::AttachmentReference>
    {
        {0, vk::ImageLayout::eColorAttachmentOptimal}
    });
    depthReference = vk::AttachmentReference{1, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    pRenderPass = Render::CRenderPass::Builder().
    addAttachmentDescription(CDevice::inst()->getImageFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR).
    addAttachmentDescription(CImage::getDepthFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal). //Depth stencil
    addSubpassDescription(vk::PipelineBindPoint::eGraphics, outReferences[0], &depthReference).
    addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eMemoryRead, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite).
    addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe,
    vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eMemoryRead).
    build();
    
    pRenderPass->pushSubpass(std::make_shared<CImguiPass>());

    pFramebuffer = std::make_unique<CFramebuffer>();
    pFramebuffer->addImage("present_khr", CDevice::inst()->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment);
    pFramebuffer->addImage("depth_image", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment);

    pRenderPass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pFramebuffer->create(pRenderPass->get(), screenExtent);
    pRenderPass->create();
}