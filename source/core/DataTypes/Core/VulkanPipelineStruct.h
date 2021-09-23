#pragma once

namespace Engine
{
    namespace Pipeline
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
            uint32_t subpass = 0;
        };
        
        struct FVulkanPipeline
        {
            std::vector<FShaderCache> vShaderCache;
            std::vector<vk::PipelineShaderStageCreateInfo> vShaderBuffer;
            vk::Pipeline graphics;
            vk::PipelineLayout layout;

            vk::DescriptorSetLayout descriptorSetLayout;
            vk::DescriptorPool descriptorPool;
            std::vector<vk::DescriptorSet> vDescriptorSets;
        };
    }   
}