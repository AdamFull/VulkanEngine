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

    outReferences.emplace(0, std::vector<vk::AttachmentReference>
    {
        {0, vk::ImageLayout::eColorAttachmentOptimal}
    });

    pRenderPass = Render::CRenderPass::Builder().
    addAttachmentDescription(CDevice::inst()->getImageFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR).
    addSubpassDescription(vk::PipelineBindPoint::eGraphics, outReferences[0]).
    addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eMemoryRead, vk::AccessFlagBits::eColorAttachmentWrite).
    build();
    
    pRenderPass->pushSubpass(std::make_shared<CImguiPass>());

    pFramebuffer = std::make_unique<CFramebuffer>();
    pFramebuffer->addImage("present_khr", CDevice::inst()->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment);

    pRenderPass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pFramebuffer->create(pRenderPass->get(), screenExtent);
    pRenderPass->create();
}