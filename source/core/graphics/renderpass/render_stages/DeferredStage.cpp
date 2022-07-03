#include "DeferredStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/renderpass/subpasses/GBufferPass.h"
#include "graphics/renderpass/subpasses/SSAOPass.h"
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

    auto gbuffer_pass = utl::make_scope<CFramebufferNew>();
    //gbuffer_pass->setFlipViewport(VK_TRUE);
    gbuffer_pass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    gbuffer_pass->addImage("packed_tex", vk::Format::eR32G32B32A32Uint, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    gbuffer_pass->addImage("emission_tex", vk::Format::eB10G11R11UfloatPack32, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    gbuffer_pass->addImage("depth_tex", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled);
    gbuffer_pass->addOutputReference(0U, "packed_tex", "emission_tex");
    gbuffer_pass->addDescription(0U, "depth_tex");

    gbuffer_pass->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader,
    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    gbuffer_pass->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eColorAttachmentWrite);
    gbuffer_pass->addRenderer(utl::make_scope<CGBufferPass>());
    vFramebuffer.emplace_back(std::move(gbuffer_pass));

    auto ssao_pass = utl::make_scope<CFramebufferNew>();
    ssao_pass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    ssao_pass->addImage("ssao_tex", vk::Format::eR8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    ssao_pass->addOutputReference(0U, "ssao_tex");
    ssao_pass->addDescription(0U);

    ssao_pass->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eTopOfPipe,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite);
    ssao_pass->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eShaderRead);  

    ssao_pass->addRenderer(utl::make_scope<CSSAOPass>());
    vFramebuffer.emplace_back(std::move(ssao_pass));

    auto composition_pass = utl::make_scope<CFramebufferNew>();
    composition_pass->setFlipViewport(VK_TRUE);
    composition_pass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    composition_pass->addImage("composition_tex", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    
    composition_pass->addOutputReference(0U, "composition_tex");
    composition_pass->addDescription(0U);

    /*composition_pass->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eTopOfPipe,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite);
    composition_pass->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eShaderRead);*/

    composition_pass->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eTopOfPipe,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite);
    composition_pass->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eShaderRead);
    composition_pass->addRenderer(utl::make_scope<CPBRCompositionPass>());
    vFramebuffer.emplace_back(std::move(composition_pass));
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