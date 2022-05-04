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

void CDeferredStage::create(std::shared_ptr<Scene::CRenderObject>& root)
{
    screenExtent = CSwapChain::getInstance()->getExtent();

    std::vector<vk::AttachmentReference> vReferences_0
    {
        {0, vk::ImageLayout::eColorAttachmentOptimal},
        {1, vk::ImageLayout::eColorAttachmentOptimal},
        {2, vk::ImageLayout::eColorAttachmentOptimal},
        {3, vk::ImageLayout::eColorAttachmentOptimal},
        {4, vk::ImageLayout::eColorAttachmentOptimal},
        {5, vk::ImageLayout::eColorAttachmentOptimal},
        {6, vk::ImageLayout::eColorAttachmentOptimal}
    };
    vk::AttachmentReference depthReference{8, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    std::vector<vk::AttachmentReference> vReferences_1
    {
        {0, vk::ImageLayout::eColorAttachmentOptimal},
        {7, vk::ImageLayout::eColorAttachmentOptimal}
    };

    std::vector<vk::AttachmentReference> vInputReferences_1
    {
        {1, vk::ImageLayout::eShaderReadOnlyOptimal},
        {2, vk::ImageLayout::eShaderReadOnlyOptimal},
        {3, vk::ImageLayout::eShaderReadOnlyOptimal},
        {4, vk::ImageLayout::eShaderReadOnlyOptimal},
        {5, vk::ImageLayout::eShaderReadOnlyOptimal},
        {6, vk::ImageLayout::eShaderReadOnlyOptimal}
    };

    pRenderPass = Render::CRenderPass::Builder().
    //KHR color attachment
    addAttachmentDescription(CSwapChain::getInstance()->getImageFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR).
    addAttachmentDescription(vk::Format::eR16G16B16A16Sfloat). //Position buffer
    addAttachmentDescription(vk::Format::eR16G16B16A16Sfloat). //Light mask buffer
    addAttachmentDescription(vk::Format::eR16G16B16A16Sfloat). //Normals buffer
    addAttachmentDescription(vk::Format::eR8G8B8A8Snorm). //Albedo buffer
    addAttachmentDescription(vk::Format::eR8G8B8A8Snorm). //Emissive buffer
    addAttachmentDescription(vk::Format::eR8G8B8A8Snorm). //Metal, roughness, AmbientOcclusion, Height maps buffer
    addAttachmentDescription(vk::Format::eR16G16B16A16Sfloat). //Temporary image buffer
    addAttachmentDescription(CImage::getDepthFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal). //Depth stencil
    //GBuffer description
    addSubpassDescription(vk::PipelineBindPoint::eGraphics, vReferences_0, &depthReference).
    //PBR composition description
    addSubpassDescription(vk::PipelineBindPoint::eGraphics, vReferences_1, &depthReference, vInputReferences_1).
    //Begin drawing gbuffer
    addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eMemoryRead, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite).
    //Begin drawing first composition
    addSubpassDependency(0, 1).
    //Draw final composition
    addSubpassDependency(1, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe,
    vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eMemoryRead).
    build();

    pRenderPass->pushSubpass(std::make_shared<CGBufferPass>());
    pRenderPass->pushSubpass(std::make_shared<CPBRCompositionPass>());

    pFramebuffer = std::make_unique<CFramebuffer>();
    pFramebuffer->addImage("present_khr", CSwapChain::getInstance()->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("position_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("lightning_mask_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("normal_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("albedo_tex", vk::Format::eR8G8B8A8Snorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("emission_tex", vk::Format::eR8G8B8A8Snorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("mrah_tex", vk::Format::eR8G8B8A8Snorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("brightness_buffer", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("depth_image", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eInputAttachment);

    pRenderPass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pFramebuffer->create(pRenderPass->get(), screenExtent);
    pRenderPass->create(root);
}

void CDeferredStage::reCreate()
{
    screenExtent = CSwapChain::getInstance()->getExtent();
    pRenderPass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pRenderPass->reCreate();
    pFramebuffer->reCreate(pRenderPass->get());
}
