#pragma once

namespace Engine
{
    struct FPipelineCreateInfo
    {
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
        vk::Viewport viewport;
        vk::Rect2D scissor;
        vk::PipelineRasterizationStateCreateInfo rasterizer;
        vk::PipelineMultisampleStateCreateInfo multisampling;
        vk::PipelineColorBlendAttachmentState colorBlendAttachment;
        vk::PipelineColorBlendStateCreateInfo colorBlending;
        vk::PipelineDepthStencilStateCreateInfo depthStencil;
        vk::PipelineBindPoint bindPoint;
    };

    class PipelineConfig
    {
    public:
        static FPipelineCreateInfo CreatePipelineConfig(uint32_t width, uint32_t height, vk::PrimitiveTopology topology, 
                                                           vk::PolygonMode polygonMode, vk::CullModeFlags cullMode,
                                                           vk::FrontFace fontFace, vk::SampleCountFlagBits samples);
        static FPipelineCreateInfo CreateDefaultPipelineConfig(uint32_t width, uint32_t height, vk::SampleCountFlagBits samples);
        static FPipelineCreateInfo CreateDefaultDebugPipelineConfig(uint32_t width, uint32_t height, vk::SampleCountFlagBits samples);
    };
}