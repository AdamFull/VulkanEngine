#include "ShadowMappingStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/renderpass/subpasses/shadow/CascadeShadowPass.h"
#include "graphics/renderpass/subpasses/shadow/DirectShadowPass.h"
#include "graphics/renderpass/subpasses/shadow/OmniShadowPass.h"

using namespace engine::core;
using namespace engine::core::render;

CShadowMappingStage::~CShadowMappingStage()
{
    cleanup();
}

void CShadowMappingStage::create()
{
    screenExtent = vk::Extent2D{SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION};

    //Cascade shadow pass
    auto cascade_fb = make_scope<CFramebufferNew>();
    //cascade_fb->setFlipViewport(VK_TRUE);
    cascade_fb->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    cascade_fb->addImage("cascade_shadowmap_tex", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArray2D, SHADOW_MAP_CASCADE_COUNT);
    cascade_fb->addDescription(0U, true);

    cascade_fb->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    cascade_fb->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eShaderRead);

    cascade_fb->addRenderer(make_scope<CCascadeShadowPass>());
    vFramebuffer.emplace_back(std::move(cascade_fb));

    //Directional shadow pass
    auto direct_fb = make_scope<CFramebufferNew>();
    direct_fb->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    direct_fb->addImage("direct_shadowmap_tex", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArray2D, MAX_SPOT_LIGHT_COUNT);
    direct_fb->addDescription(0U, true);

    direct_fb->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    direct_fb->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eShaderRead);

    direct_fb->addRenderer(make_scope<CDirectShadowPass>());
    vFramebuffer.emplace_back(std::move(direct_fb));

    //Point shadow pass
    auto omni_fb = make_scope<CFramebufferNew>();
    //omni_fb->setFlipViewport(VK_TRUE);
    omni_fb->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    omni_fb->addImage("omni_shadowmap_tex", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArrayCube, MAX_POINT_LIGHT_COUNT);
    omni_fb->addDescription(0U, true);

    omni_fb->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    omni_fb->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eShaderRead);

    omni_fb->addRenderer(make_scope<COmniShadowPass>());
    vFramebuffer.emplace_back(std::move(omni_fb));

    CRenderStage::create();
}