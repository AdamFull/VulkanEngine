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

                eBRDF,
                eIrradiateCube,
                ePrefiltred,

                eDeferred
            };

            struct FPipelineCreateInfo
            {
                vk::VertexInputBindingDescription vertexInputDesc;
                std::vector<vk::VertexInputAttributeDescription> vertexAtribDesc;
                vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
                vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
                vk::Viewport viewport;
                vk::Rect2D scissor;
                vk::PipelineRasterizationStateCreateInfo rasterizer;
                vk::PipelineMultisampleStateCreateInfo multisampling;
                std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;
                vk::PipelineColorBlendStateCreateInfo colorBlending;
                vk::PipelineDepthStencilStateCreateInfo depthStencil;
                std::vector<vk::DynamicState> dynamicStateEnables;
                vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
                vk::PipelineBindPoint bindPoint;
                vk::PipelineLayout pipelineLayout;
                vk::PipelineCache pipelineCache;
                vk::RenderPass renderPass;

                EPipelineType eType;
                EShaderSet eSet;
            };

            class PipelineConfig
            {
            public:
                static FPipelineCreateInfo CreateUIPipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache = VK_NULL_HANDLE);
                static FPipelineCreateInfo CreateDiffusePipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache = VK_NULL_HANDLE);
                static FPipelineCreateInfo CreateSkyboxPipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache = VK_NULL_HANDLE);

                //PBR
                static FPipelineCreateInfo CreateBRDFPipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache = VK_NULL_HANDLE);
                static FPipelineCreateInfo CreateIrradiatePipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache = VK_NULL_HANDLE);
                static FPipelineCreateInfo CreatePrefiltredPipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache = VK_NULL_HANDLE);

                //Deferred
                static FPipelineCreateInfo CreateDeferredPipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache = VK_NULL_HANDLE);
            };
        }
    }
}