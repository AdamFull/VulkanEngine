#pragma once

namespace Engine
{
    namespace Core
    {
        struct Initializers
        {
            static vk::PipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCI(vk::PrimitiveTopology topology, vk::Bool32 primitiveRestart);
            static vk::PipelineRasterizationStateCreateInfo PipelineRasterizerStateCI(vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace, vk::Bool32 bDepthBias = VK_FALSE);
            static vk::PipelineColorBlendAttachmentState PipelineColorBlendAttachmentState(vk::ColorComponentFlags flags, vk::Bool32 blend);
            static vk::PipelineDepthStencilStateCreateInfo PipelineDepthStencilStateCI(vk::Bool32 depthTestEnable, vk::Bool32 depthWriteEnable, vk::CompareOp depthCompareOp);
            static vk::Viewport Viewport(int32_t width, int32_t height, float x = 0.f, float y = 0.f);
            static vk::Rect2D Scissor(int32_t width, int32_t height, int32_t x = 0, int32_t y = 0);
        };
    }
}