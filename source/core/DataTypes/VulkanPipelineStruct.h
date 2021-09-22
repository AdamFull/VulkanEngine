#pragma once

namespace Engine::Pipeline
{
    struct FShaderCache
    {
        vk::ShaderStageFlagBits sShaderType;
        std::vector<char> srShaderData;
    };

    struct FPipelineConfigInfo 
    {
        vk::Viewport viewport;
        vk::Rect2D scissor;
        vk::PipelineViewportStateCreateInfo viewportInfo;
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
        vk::PipelineMultisampleStateCreateInfo multisampleInfo;
        vk::PipelineColorBlendAttachmentState colorBlendAttachment;
        vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
        vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
        vk::PipelineLayout pipelineLayout = nullptr;
        uint32_t subpass = 0;
    };
    
    struct FVulkanPipeline
    {
        std::vector<FShaderCache> vShaderCache;
        std::vector<vk::PipelineShaderStageCreateInfo> vShaderBuffer;
        vk::Pipeline graphicsPipeline;
    };
    
}