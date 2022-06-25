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
    screenExtent = vk::Extent2D{1024, 1024};

    auto cascade_fb = make_scope<CFramebufferNew>();
    //cascade_fb->setFlipViewport(VK_TRUE);
    cascade_fb->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    cascade_fb->addImage("cascade_shadowmap_tex", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArray2D, SHADOW_MAP_CASCADE_COUNT);
    cascade_fb->addDescription(0U, true);

    cascade_fb->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllGraphics,
    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite);
    cascade_fb->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eAllGraphics, vk::PipelineStageFlagBits::eAllCommands,
    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite);

    cascade_fb->addRenderer(make_scope<CCascadeShadowPass>());
    //vFramebuffer.emplace_back(std::move(cascade_fb));

    auto direct_fb = make_scope<CFramebufferNew>();
    //direct_fb->setFlipViewport(VK_TRUE);
    direct_fb->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    direct_fb->addImage("direct_shadowmap_tex", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArray2D, MAX_SPOT_LIGHT_COUNT);
    direct_fb->addDescription(0U, true);

    direct_fb->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllGraphics,
    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite);
    direct_fb->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eAllGraphics, vk::PipelineStageFlagBits::eAllCommands,
    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite);

    direct_fb->addRenderer(make_scope<CDirectShadowPass>());
    vFramebuffer.emplace_back(std::move(direct_fb));

    CRenderStage::create();
}