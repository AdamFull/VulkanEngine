#include "ShadowMappingStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/renderpass/subpasses/ShadowPass.h"

using namespace engine::core;
using namespace engine::core::render;

CShadowMappingStage::~CShadowMappingStage()
{
    cleanup();
}

void CShadowMappingStage::create()
{
    screenExtent = vk::Extent2D{1024, 1024};

    auto framebuffer_0 = make_scope<CFramebufferNew>();
    //framebuffer_0->setFlipViewport(VK_TRUE);
    framebuffer_0->setRenderArea(vk::Offset2D{0, 0}, screenExtent);

    //framebuffer_0->addOutputReference(0U, 0U);
    framebuffer_0->addImage("shadowmap_tex", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArray2D, 16);
    framebuffer_0->addDescription(0U, true);

    framebuffer_0->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllGraphics,
    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite);
    framebuffer_0->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eAllGraphics, vk::PipelineStageFlagBits::eAllCommands,
    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite);

    framebuffer_0->addRenderer(make_scope<CShadowPass>());
    vFramebuffer.emplace_back(std::move(framebuffer_0));
    CRenderStage::create();
}