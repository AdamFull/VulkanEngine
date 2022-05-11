#include "DeferredStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/renderpass/subpasses/GBufferPass.h"
#include "graphics/renderpass/subpasses/PBRCompositionPass.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;

CDeferredStage::~CDeferredStage()
{
    cleanup();
}

void CDeferredStage::create()
{
    screenExtent = CDevice::inst()->getExtent();

    outReferences.emplace(0, std::vector<vk::AttachmentReference>
    {
        {1, vk::ImageLayout::eColorAttachmentOptimal},
        {2, vk::ImageLayout::eColorAttachmentOptimal},
        {3, vk::ImageLayout::eColorAttachmentOptimal},
        {4, vk::ImageLayout::eColorAttachmentOptimal},
        {5, vk::ImageLayout::eColorAttachmentOptimal},
        {6, vk::ImageLayout::eColorAttachmentOptimal},
    });
    outReferences.emplace(1, std::vector<vk::AttachmentReference>
    {
        {0, vk::ImageLayout::eColorAttachmentOptimal},
        {7, vk::ImageLayout::eColorAttachmentOptimal}
    });
    depthReference = vk::AttachmentReference{8, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    inReferences.emplace(1, std::vector<vk::AttachmentReference>
    {
        {1, vk::ImageLayout::eShaderReadOnlyOptimal},
        {2, vk::ImageLayout::eShaderReadOnlyOptimal},
        {3, vk::ImageLayout::eShaderReadOnlyOptimal},
        {4, vk::ImageLayout::eShaderReadOnlyOptimal},
        {5, vk::ImageLayout::eShaderReadOnlyOptimal},
        {6, vk::ImageLayout::eShaderReadOnlyOptimal},
    });

    pRenderPass = Render::CRenderPass::Builder().
    addAttachmentDescription(vk::Format::eR32G32B32A32Sfloat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal). //Final image
    addAttachmentDescription(vk::Format::eR16G16B16A16Sfloat). //Position buffer
    addAttachmentDescription(vk::Format::eR16G16B16A16Sfloat). //Light mask buffer
    addAttachmentDescription(vk::Format::eR16G16B16A16Sfloat). //Normals buffer
    addAttachmentDescription(vk::Format::eR8G8B8A8Snorm). //Albedo buffer
    addAttachmentDescription(vk::Format::eR8G8B8A8Snorm). //Emissive buffer
    addAttachmentDescription(vk::Format::eR8G8B8A8Srgb). //Metal, roughness, AmbientOcclusion, Height maps buffer
    addAttachmentDescription(vk::Format::eR32G32B32A32Sfloat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal). //Temporary image buffer
    addAttachmentDescription(CImage::getDepthFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal). //Depth stencil
    //GBuffer description
    addSubpassDescription(vk::PipelineBindPoint::eGraphics, outReferences[0], &depthReference).
    //PBR composition description
    addSubpassDescription(vk::PipelineBindPoint::eGraphics, outReferences[1], nullptr, inReferences[1]).
    /*addSubpassDependency(0, 1, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead).
    addSubpassDependency(0, 1, vk::PipelineStageFlagBits::eEarlyFragmentTests /*| vk::PipelineStageFlagBits::eLateFragmentTests, 
    vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eShaderRead).
    addSubpassDependency(1, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eMemoryRead).*/
    addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader,
    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite).
    addSubpassDependency(0, 1, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
    vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eColorAttachmentWrite).
    addSubpassDependency(1, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eMemoryRead).
    build();

    pRenderPass->pushSubpass(std::make_shared<CGBufferPass>());
    pRenderPass->pushSubpass(std::make_shared<CPBRCompositionPass>());

    pFramebuffer = std::make_unique<CFramebuffer>();
    pFramebuffer->addImage("output_color", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    pFramebuffer->addImage("position_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("lightning_mask_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("normal_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("albedo_tex", vk::Format::eR8G8B8A8Snorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("emission_tex", vk::Format::eR8G8B8A8Snorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("mrah_tex", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment);
    pFramebuffer->addImage("brightness_buffer", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    pFramebuffer->addImage("depth_image", CImage::getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment);

    pRenderPass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pFramebuffer->create(pRenderPass->get(), screenExtent);
    pRenderPass->create();
}
