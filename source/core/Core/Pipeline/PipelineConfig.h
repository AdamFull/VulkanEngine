#pragma once

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            struct FPipelineInitial
            {
                vk::RenderPass renderPass{VK_NULL_HANDLE};
                vk::PipelineLayout pipelineLayout{VK_NULL_HANDLE};
                vk::PipelineCache pipelineCache{VK_NULL_HANDLE};
                vk::PipelineBindPoint bindPoint{vk::PipelineBindPoint::eGraphics};
                uint32_t color_attachments{1};
                vk::VertexInputBindingDescription vertexInputDesc;
                std::vector<vk::VertexInputAttributeDescription> vertexAtribDesc;
                vk::CullModeFlagBits culling{vk::CullModeFlagBits::eNone};
                vk::FrontFace font_face{vk::FrontFace::eCounterClockwise};
                vk::Bool32 enableDepth{VK_FALSE};
                std::vector<vk::DynamicState> dynamicStateEnables{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
                std::vector<std::string> shaders;
            };

            struct FPipelineCreateInfo
            {
                vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
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
                std::vector<std::string> shaders;
            };

            class PipelineConfig
            {
            public:
                static FPipelineCreateInfo MakeInfo(const FPipelineInitial& initial);
            };
        }
    }
}