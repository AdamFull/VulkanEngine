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
    screenExtent = CDevice::inst()->getExtent();

    outReferences.emplace(0, std::vector<vk::AttachmentReference>
    {
        {0, vk::ImageLayout::eColorAttachmentOptimal}
    });

    pRenderPass = Render::CRenderPass::Builder().
    addAttachmentDescription(vk::Format::eR8G8B8A8Unorm, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal).
    addSubpassDescription(vk::PipelineBindPoint::eGraphics, outReferences[0]).    //First blur pass
    addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite).
    addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead).
    build();

    pRenderPass->pushSubpass(std::make_shared<CFinalCompositionPass>());

    pFramebuffer = std::make_unique<CFramebuffer>();
    pFramebuffer->addImage("output_color", vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);

    pRenderPass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pFramebuffer->create(pRenderPass->get(), screenExtent);
    pRenderPass->create();
}