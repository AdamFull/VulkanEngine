#include "PostProcessStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/renderpass/subpasses/FinalCompositionPass.h"
#include "graphics/renderpass/subpasses/ThroughPass.h"

using namespace engine::core;
using namespace engine::core::render;

CPostProcessStage::~CPostProcessStage()
{
    cleanup();
}

void CPostProcessStage::create()
{
    detectExtent = true;
    screenExtent = CDevice::inst()->getExtent(detectExtent);

    std::string bloomTexture{"composition_tex"};

    auto framebuffer_6 = utl::make_scope<CFramebufferNew>();
    framebuffer_6->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    framebuffer_6->addImage("output_color", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
    framebuffer_6->addOutputReference(0U, "output_color");
    framebuffer_6->addDescription(0U);
    framebuffer_6->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);
    framebuffer_6->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead);
    framebuffer_6->addRenderer(utl::make_scope<CFinalCompositionPass>(bloomTexture));
    vFramebuffer.emplace_back(std::move(framebuffer_6));

    /*auto framebuffer_7 = utl::make_scope<CFramebufferNew>();
    framebuffer_7->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    framebuffer_7->addImage("output_color", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
    framebuffer_7->addOutputReference(0U, "output_color");
    framebuffer_7->addDescription(0U);
    framebuffer_7->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);
    framebuffer_7->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead);
    framebuffer_7->addRenderer(utl::make_scope<CThroughPass>(bloomTexture));
    vFramebuffer.emplace_back(std::move(framebuffer_7));*/

    CRenderStage::create();
}

//TODO: make it better
void CPostProcessStage::rebuild()
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