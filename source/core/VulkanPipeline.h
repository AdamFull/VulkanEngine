#pragma once
#include "VulkanDevice.h"

namespace Engine
{
    constexpr const char* srShaderEntriePoint = "main";

    struct FShaderCache
    {
        bool operator==(FShaderCache &rhs)
        {
            return rhs.sShaderType == this->sShaderType &&
                   rhs.srShaderData == this->srShaderData;
        }

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
        vk::RenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class VulkanPipeline
    {
    public:
        using shader_load_map_t = std::map<vk::ShaderStageFlagBits, std::string>;

        VulkanPipeline(std::unique_ptr<VulkanDevice>& device, const FPipelineConfigInfo& configInfo);

        void LoadShader(const std::string& srShaderPath, vk::ShaderStageFlagBits fShaderType);
        void LoadShader(const shader_load_map_t& mShaders);
    private:
        void RecreateShaders();
        void LoadShader(const std::vector<char>& vShaderCode, vk::ShaderStageFlagBits fShaderType);
    private:
        std::vector<FShaderCache> m_vShaderCache;
        std::vector<vk::PipelineShaderStageCreateInfo> m_vShaderBuffer;
    };
}