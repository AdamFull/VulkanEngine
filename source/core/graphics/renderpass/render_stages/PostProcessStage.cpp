#include "PostProcessStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/renderpass/subpasses/GaussianBlurPass.h"
#include "graphics/renderpass/subpasses/FinalCompositionPass.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;

CPostProcessStage::~CPostProcessStage()
{
    cleanup();
}

void CPostProcessStage::create()
{
    screenExtent = CSwapChain::getInstance()->getExtent();

    std::vector<vk::AttachmentReference> vReferences_0
    {
        {0, vk::ImageLayout::eColorAttachmentOptimal}
    };
    vk::AttachmentReference depthReference{1, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    std::vector<vk::AttachmentReference> vReferences_1
    {
        {2, vk::ImageLayout::eColorAttachmentOptimal}
    };

    std::vector<vk::AttachmentReference> vReferences_2
    {
        {0, vk::ImageLayout::eColorAttachmentOptimal}
    };

    pRenderPass = Render::CRenderPass::Builder().
    addAttachmentDescription(CSwapChain::getInstance()->getImageFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR).
    /*addAttachmentDescription(vk::Format::eR16G16B16A16Sfloat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal).*/  // Temporary image buffer
    /*addAttachmentDescription(vk::Format::eR16G16B16A16Sfloat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal).*/ // Temporary image buffer
    addAttachmentDescription(CImage::getDepthFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal). //Depth stencil
    addSubpassDescription(vk::PipelineBindPoint::eGraphics, vReferences_0, &depthReference).    //First blur pass
    //addSubpassDescription(vk::PipelineBindPoint::eGraphics, vReferences_1, &depthReference).    //Second blur pass
    //addSubpassDescription(vk::PipelineBindPoint::eGraphics, vReferences_2, &depthReference).    //Composition pass
    addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eMemoryRead, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite).
    //addSubpassDependency(0, 1).
    //addSubpassDependency(1, 2).
    addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe,
    vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eMemoryRead).
    build();

    // final image pass?
    //pRenderPass->pushSubpass(std::make_shared<CGaussianBlurPass>(-1, "brightness_buffer"));
    //pRenderPass->pushSubpass(std::make_shared<CGaussianBlurPass>(1, "brightness_buffer"));
    pRenderPass->pushSubpass(std::make_shared<CFinalCompositionPass>());

    pFramebuffer = std::make_unique<CFramebuffer>();
    pFramebuffer->addImage("present_khr", CSwapChain::getInstance()->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment);
    //pFramebuffer->addImage("first_pass", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    //pFramebuffer->addImage("bloom_image", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    pFramebuffer->addImage("depth_image", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment);

    pRenderPass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pFramebuffer->create(pRenderPass->get(), screenExtent);
    pRenderPass->create();
}

void CPostProcessStage::reCreate()
{
    screenExtent = CSwapChain::getInstance()->getExtent();
    pRenderPass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pRenderPass->reCreate();
    pFramebuffer->reCreate(pRenderPass->get());
}