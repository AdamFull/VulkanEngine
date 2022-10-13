#include "RaymarchStage.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/renderpass/subpasses/VoxelPass.h"

using namespace engine::core;
using namespace engine::core::render;

void CRaymarchStage::create()
{
    detectExtent = true;
    screenExtent = UDevice->getExtent(detectExtent);

    auto voxel_fb = utl::make_scope<CFramebufferNew>();
    voxel_fb->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    voxel_fb->addImage("output_color", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
    voxel_fb->addOutputReference(0U, "output_color");
    voxel_fb->addDescription(0U);
    voxel_fb->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite);
    voxel_fb->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead);
    voxel_fb->addRenderer(utl::make_scope<CVoxelPass>());
    vFramebuffer.emplace_back(std::move(voxel_fb));

    CRenderStage::create();
}