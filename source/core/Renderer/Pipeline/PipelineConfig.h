#pragma once

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            enum class EPipelineType
            {
                eNone = -1,
                eGraphics
            };

            enum class EShaderSet
            {
                eNone = -1,
                eUI,
                eDiffuse,
                eSkybox,
                ePBR
            };

            struct FPipelineCreateInfo
            {
                vk::VertexInputBindingDescription vertexInputDesc;
                std::vector<vk::VertexInputAttributeDescription> vertexAtribDesc;
                vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
                vk::Viewport viewport;
                vk::Rect2D scissor;
                vk::PipelineRasterizationStateCreateInfo rasterizer;
                vk::PipelineMultisampleStateCreateInfo multisampling;
                vk::PipelineColorBlendAttachmentState colorBlendAttachment;
                vk::PipelineColorBlendStateCreateInfo colorBlending;
                vk::PipelineDepthStencilStateCreateInfo depthStencil;
                std::vector<vk::DynamicState> dynamicStateEnables;
                vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
                vk::PipelineBindPoint bindPoint;
                vk::PipelineLayout pipelineLayout;
                vk::PipelineCache pipelineCache;

                EPipelineType eType;
                EShaderSet eSet;
            };

            class PipelineConfig
            {
            public:
                static FPipelineCreateInfo CreateUIPipeline(vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache = VK_NULL_HANDLE);
                static FPipelineCreateInfo CreateDiffusePipeline(vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache = VK_NULL_HANDLE);
                static FPipelineCreateInfo CreateSkyboxPipeline(vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache = VK_NULL_HANDLE);
                static FPipelineCreateInfo CreatePBRPipeline(vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache = VK_NULL_HANDLE);
            };
        }
    }
}