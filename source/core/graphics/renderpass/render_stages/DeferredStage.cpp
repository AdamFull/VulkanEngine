#include "DeferredStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/renderpass/subpasses/GBufferPass.h"
#include "graphics/renderpass/subpasses/SSAOPass.h"
#include "graphics/renderpass/subpasses/SSRPass.h"
#include "graphics/renderpass/subpasses/PBRCompositionPass.h"

using namespace engine::core;
using namespace engine::core::render;

void CDeferredStage::create()
{
    detectExtent = true;
    screenExtent = UDevice->getExtent(detectExtent);

    auto gbuffer_pass = utl::make_scope<CFramebufferNew>();
    //gbuffer_pass->setFlipViewport(VK_TRUE);
    gbuffer_pass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    gbuffer_pass->addImage("packed_tex", vk::Format::eR32G32B32A32Uint, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    gbuffer_pass->addImage("velocity_tex", vk::Format::eR8G8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    gbuffer_pass->addImage("emission_tex", vk::Format::eB10G11R11UfloatPack32, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    gbuffer_pass->addImage("depth_tex", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled);
    gbuffer_pass->addOutputReference(0U, "packed_tex", "emission_tex", "velocity_tex");
    gbuffer_pass->addDescription(0U, "depth_tex");

    gbuffer_pass->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader,
    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    gbuffer_pass->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eColorAttachmentWrite);
    gbuffer_pass->addRenderer(utl::make_scope<CGBufferPass>());
    vFramebuffer.emplace_back(std::move(gbuffer_pass));

    auto ssr_pass = utl::make_scope<CFramebufferNew>();
    //ssr_pass->setFlipViewport(VK_TRUE);
    ssr_pass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    ssr_pass->addImage("ssr_tex", vk::Format::eB10G11R11UfloatPack32, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    ssr_pass->addOutputReference(0U, "ssr_tex");
    ssr_pass->addDescription(0U);

    ssr_pass->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eTopOfPipe,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite);
    ssr_pass->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eShaderRead);  

    ssr_pass->addRenderer(utl::make_scope<CSSRPass>());
    vFramebuffer.emplace_back(std::move(ssr_pass));

    auto composition_pass = utl::make_scope<CFramebufferNew>();
    composition_pass->setFlipViewport(VK_TRUE);
    composition_pass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    composition_pass->addImage("composition_tex", vk::Format::eB10G11R11UfloatPack32, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    
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
    screenExtent = UDevice->getExtent(detectExtent);
    framebufferIndex = 0;
    for(auto& framebuffer : vFramebuffer)
    {
        framebuffer->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
        framebuffer->reCreate();
        framebufferIndex++;
    }
}