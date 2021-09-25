#pragma once
#include "DataTypes/VulkanVertex.h"

namespace Engine
{
    struct FShaderCache
    {
        vk::ShaderStageFlagBits sShaderType;
        std::vector<char> srShaderData;
    };

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

    struct FPipeline
    {
        vk::PipelineLayout layout;
        vk::Pipeline pipeline;
    };

    class PipelineBase
    {
    public:
        PipelineBase();
        ~PipelineBase();

        void Create(FPipelineCreateInfo createInfo, vk::UniqueDevice& device, vk::DescriptorSetLayout& descriptorSetLayout, vk::RenderPass& renderPass);

        static FPipelineCreateInfo CreatePipelineConfig(uint32_t width, uint32_t height, vk::PrimitiveTopology topology, 
                                                           vk::PolygonMode polygonMode, vk::CullModeFlags cullMode,
                                                           vk::FrontFace fontFace, vk::SampleCountFlagBits samples);
        static FPipelineCreateInfo CreateDefaultPipelineConfig(uint32_t width, uint32_t height, vk::SampleCountFlagBits samples);
        static FPipelineCreateInfo CreateDefaultDebugPipelineConfig(uint32_t width, uint32_t height, vk::SampleCountFlagBits samples);

        PipelineBase(const PipelineBase&) = delete;
        void operator=(const PipelineBase&) = delete;
        PipelineBase(PipelineBase&&) = delete;
        PipelineBase& operator=(PipelineBase&&) = delete;

        void Bind(vk::CommandBuffer& commandBuffer);
        void PushConstants(vk::CommandBuffer& commandBuffer, FUniformData& ubo);

        void LoadShader(vk::UniqueDevice& device, const std::map<vk::ShaderStageFlagBits, std::string>& mShaders);
        void RecreatePipeline(vk::UniqueDevice& device, vk::DescriptorSetLayout& descriptorSetLayout, vk::RenderPass& renderPass, vk::Extent2D swapChainExtent);

        void Cleanup(vk::UniqueDevice& device);

        FPipeline& get() { return data; }

    private:
        void CreatePipelineLayout(vk::UniqueDevice& device, vk::DescriptorSetLayout& descriptorSetLayout);
        void CreatePipeline(vk::UniqueDevice& device, vk::RenderPass renderPass);

        void LoadShader(vk::UniqueDevice& device, const std::string& srShaderPath, vk::ShaderStageFlagBits fShaderType);
        void LoadShader(vk::UniqueDevice& device, const std::vector<char>& vShaderCode, vk::ShaderStageFlagBits fShaderType);
        void RecreateShaders(vk::UniqueDevice& device);
    private:
        std::vector<FShaderCache> m_vShaderCache;
        std::vector<vk::PipelineShaderStageCreateInfo> m_vShaderBuffer;
        FPipelineCreateInfo savedInfo;

        FPipeline data;
    };
};