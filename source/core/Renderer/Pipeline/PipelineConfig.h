#pragma once

namespace Engine
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
        static FPipelineCreateInfo CreatePipelineConfig(EPipelineType eType, EShaderSet eSet, 
                                                        vk::PrimitiveTopology topology, vk::PolygonMode polygonMode, 
                                                        vk::CullModeFlags cullMode, vk::FrontFace fontFace, 
                                                        vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout,
                                                        vk::PipelineCache pipelineCache);
        static FPipelineCreateInfo CreateDefaultPipelineConfig(EPipelineType eType, EShaderSet eSet,vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache = VK_NULL_HANDLE);
        static FPipelineCreateInfo CreateDefaultDebugPipelineConfig(EPipelineType eType, EShaderSet eSet,vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache = VK_NULL_HANDLE);
    };
}