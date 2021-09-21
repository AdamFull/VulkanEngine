#pragma once

namespace Engine
{
    constexpr const char* srShaderEntriePoint = "main";

    struct FShaderCache
    {
        vk::ShaderStageFlagBits sShaderType;
        std::vector<char> srShaderData;
    };

    class VulkanDevice;

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

        VulkanPipeline(std::unique_ptr<VulkanDevice>& device);
        void CreatePipeline(const FPipelineConfigInfo& configInfo);

        static FPipelineConfigInfo PipelineDefault(uint32_t width, uint32_t height);

        VulkanPipeline(const VulkanPipeline&) = delete;
        void operator=(const VulkanPipeline&) = delete;

        void LoadShader(const std::string& srShaderPath, vk::ShaderStageFlagBits fShaderType);
        void LoadShader(const shader_load_map_t& mShaders);
    private:
        void RecreateShaders();
        void LoadShader(const std::vector<char>& vShaderCode, vk::ShaderStageFlagBits fShaderType);
    private:
        std::vector<FShaderCache> m_vShaderCache;
        std::vector<vk::PipelineShaderStageCreateInfo> m_vShaderBuffer;
        vk::Pipeline m_GraphicsPipeline;
        vk::Device m_Device;
    };
}