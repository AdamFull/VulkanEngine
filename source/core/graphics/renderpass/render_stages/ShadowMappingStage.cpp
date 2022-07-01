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

    auto shadows_fb = make_scope<CFramebufferNew>();
    shadows_fb->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    shadows_fb->addImage("cascade_shadowmap_tex", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArray2D, SHADOW_MAP_CASCADE_COUNT);
    shadows_fb->addDescription(0U, "cascade_shadowmap_tex");
    shadows_fb->addRenderer(make_scope<CCascadeShadowPass>());

    shadows_fb->addImage("direct_shadowmap_tex", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArray2D, MAX_SPOT_LIGHT_COUNT);
    shadows_fb->addDescription(1U, "direct_shadowmap_tex");
    shadows_fb->addRenderer(make_scope<CDirectShadowPass>());

    shadows_fb->addImage("omni_shadowmap_tex", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArrayCube, MAX_POINT_LIGHT_COUNT);
    shadows_fb->addDescription(2U, "omni_shadowmap_tex");
    shadows_fb->addRenderer(make_scope<COmniShadowPass>());

    shadows_fb->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    shadows_fb->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eShaderRead);
    shadows_fb->addSubpassDependency(1, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eShaderRead);
    shadows_fb->addSubpassDependency(2, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eShaderRead);

    vFramebuffer.emplace_back(std::move(shadows_fb));

    CRenderStage::create();
}