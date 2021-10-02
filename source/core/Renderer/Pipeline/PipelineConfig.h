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
        vk::PipelineLayout pipelineLayout;
    };

    class PipelineConfig
    {
    public:
        static FPipelineCreateInfo CreatePipelineConfig(vk::PrimitiveTopology topology, vk::PolygonMode polygonMode, 
                                                        vk::CullModeFlags cullMode, vk::FrontFace fontFace, 
                                                        vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout);
        static FPipelineCreateInfo CreateDefaultPipelineConfig(vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout);
        static FPipelineCreateInfo CreateDefaultDebugPipelineConfig(vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout);
    };
}