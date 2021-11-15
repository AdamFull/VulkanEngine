#include "VulkanInitializers.h"

using namespace Engine::Core;

vk::PipelineInputAssemblyStateCreateInfo Initializers::PipelineInputAssemblyStateCI(vk::PrimitiveTopology topology, vk::Bool32 primitiveRestart)
{
    vk::PipelineInputAssemblyStateCreateInfo createInfo{};
    createInfo.topology = topology;
    createInfo.flags = vk::PipelineInputAssemblyStateCreateFlags{};
    createInfo.primitiveRestartEnable = primitiveRestart;
    return createInfo;
}

vk::PipelineRasterizationStateCreateInfo Initializers::PipelineRasterizerStateCI(vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace)
{
    vk::PipelineRasterizationStateCreateInfo createInfo{};
    createInfo.depthClampEnable = VK_FALSE;
    createInfo.rasterizerDiscardEnable = VK_FALSE;
    createInfo.polygonMode = polygonMode;
    createInfo.lineWidth = 1.0f;
    createInfo.cullMode = cullMode;
    createInfo.frontFace = frontFace;
    createInfo.depthBiasEnable = VK_FALSE;
    return createInfo;
}

vk::PipelineColorBlendAttachmentState Initializers::PipelineColorBlendAttachmentState(vk::ColorComponentFlags flags, vk::Bool32 blend)
{
    vk::PipelineColorBlendAttachmentState state{};
    state.colorWriteMask = flags;
    state.blendEnable = blend;
    state.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    state.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    state.colorBlendOp = vk::BlendOp::eAdd;
    state.srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    state.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    state.alphaBlendOp = vk::BlendOp::eAdd;
    return state;
}

vk::PipelineDepthStencilStateCreateInfo Initializers::PipelineDepthStencilStateCI(vk::Bool32 depthTestEnable, vk::Bool32 depthWriteEnable, vk::CompareOp depthCompareOp)
{
    vk::PipelineDepthStencilStateCreateInfo createInfo{};
    createInfo.depthTestEnable = depthTestEnable;
    createInfo.depthWriteEnable = depthWriteEnable;
    createInfo.depthCompareOp = depthCompareOp;
    createInfo.back.compareOp = vk::CompareOp::eAlways;
    createInfo.depthBoundsTestEnable = VK_FALSE;
    createInfo.stencilTestEnable = VK_FALSE;
    return createInfo;
}

vk::Viewport Initializers::Viewport(int32_t width, int32_t height, float x, float y)
{
    vk::Viewport viewport{};
    viewport.x = x;
    viewport.y = y;
    viewport.width = static_cast<float>(width);
    viewport.height = static_cast<float>(height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    return viewport;
}

vk::Rect2D Initializers::Scissor(int32_t width, int32_t height, int32_t x, int32_t y)
{
    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D{x, y};
    scissor.extent = vk::Extent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    return scissor;
}