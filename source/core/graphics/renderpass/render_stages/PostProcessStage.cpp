#include "PostProcessStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/renderpass/subpasses/BrightDetectPass.h"
#include "graphics/renderpass/subpasses/BlurPass.h"
#include "graphics/renderpass/subpasses/DownsamplePass.h"
#include "graphics/renderpass/subpasses/FinalCompositionPass.h"
#include "graphics/renderpass/subpasses/ScreenSpaceReflectionsPass.h"

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
    bool enable_bloom{false};
    if(enable_bloom)
    {
        //Add linear filter
        auto framebuffer_0 = make_scope<CFramebufferNew>();
        framebuffer_0->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
        framebuffer_0->addOutputReference(0U, 0U);
        framebuffer_0->addImage("brightness_buffer", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
        framebuffer_0->addDescription(0U);
        framebuffer_0->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);
        framebuffer_0->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
        vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead);
        framebuffer_0->pushSubpass(make_scope<CBrightDetectPass>());
        vFramebuffer.emplace_back(std::move(framebuffer_0));

        auto framebuffer_1 = make_scope<CFramebufferNew>();
        framebuffer_1->setRenderArea(vk::Offset2D{0, 0}, vk::Extent2D(screenExtent.width/16, screenExtent.height/16));
        framebuffer_1->addOutputReference(0U, 0U);
        framebuffer_1->addImage("downsampled_image", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
        framebuffer_1->addDescription(0U);
        framebuffer_1->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);
        framebuffer_1->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
        vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead);
        framebuffer_1->pushSubpass(make_scope<CDownsamplePass>("brightness_buffer"));
        vFramebuffer.emplace_back(std::move(framebuffer_1));

        auto framebuffer_2 = make_scope<CFramebufferNew>();
        framebuffer_2->setRenderArea(vk::Offset2D{0, 0}, vk::Extent2D(screenExtent.width/8, screenExtent.height/8));
        framebuffer_2->addOutputReference(0U, 0U);
        framebuffer_2->addImage("spdownsampled_image", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
        framebuffer_2->addDescription(0U);
        framebuffer_2->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);
        framebuffer_2->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
        vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead);
        framebuffer_2->pushSubpass(make_scope<CDownsamplePass>("downsampled_image"));
        vFramebuffer.emplace_back(std::move(framebuffer_2));

        auto framebuffer_3 = make_scope<CFramebufferNew>();
        framebuffer_3->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
        framebuffer_3->addOutputReference(0U, 0U);
        framebuffer_3->addImage("horisontal_blur_pass", vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
        framebuffer_3->addDescription(0U);
        framebuffer_3->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);
        framebuffer_3->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
        vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead);
        framebuffer_3->pushSubpass(make_scope<CBlurPass>(-1, "spdownsampled_image"));
        vFramebuffer.emplace_back(std::move(framebuffer_3));

        auto framebuffer_4 = make_scope<CFramebufferNew>();
        framebuffer_4->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
        framebuffer_4->addOutputReference(0U, 0U);
        framebuffer_4->addImage("vertical_blur_pass", vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
        framebuffer_4->addDescription(0U);
        framebuffer_4->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);
        framebuffer_4->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
        vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead);
        framebuffer_4->pushSubpass(make_scope<CBlurPass>(1, "horisontal_blur_pass"));
        vFramebuffer.emplace_back(std::move(framebuffer_4));
        bloomTexture = "horisontal_blur_pass";
    }
    //bloomTexture = "output_reflections";
    auto framebuffer_5 = make_scope<CFramebufferNew>();
    framebuffer_5->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    framebuffer_5->addOutputReference(0U, 0U);
    framebuffer_5->addImage("output_color", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    framebuffer_5->addDescription(0U);
    framebuffer_5->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);
    framebuffer_5->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead);
    framebuffer_5->pushSubpass(make_scope<CScreenSpaceReflectionsPass>());
    vFramebuffer.emplace_back(std::move(framebuffer_5));

    /*auto framebuffer_6 = make_scope<CFramebufferNew>();
    framebuffer_6->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    framebuffer_6->addOutputReference(0U, 0U);
    framebuffer_6->addImage("output_color", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
    framebuffer_6->addDescription(0U);
    framebuffer_6->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);
    framebuffer_6->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead);
    framebuffer_6->pushSubpass(make_scope<CFinalCompositionPass>(bloomTexture));
    vFramebuffer.emplace_back(std::move(framebuffer_6));*/

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