#pragma once
#include "DataTypes/VulkanVertex.h"

namespace Engine
{
    class Device;
    class SwapChain;

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

        void Create(FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, vk::DescriptorSetLayout& descriptorSetLayout);

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

        void LoadShader(std::unique_ptr<Device>& device, const std::map<vk::ShaderStageFlagBits, std::string>& mShaders);
        void RecreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, vk::DescriptorSetLayout& descriptorSetLayout);

        void Cleanup(std::unique_ptr<Device>& device);

        FPipeline& get() { return data; }

    private:
        void CreatePipelineLayout(std::unique_ptr<Device>& device, vk::DescriptorSetLayout& descriptorSetLayout);
        void CreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);

        void LoadShader(std::unique_ptr<Device>& device, const std::string& srShaderPath, vk::ShaderStageFlagBits fShaderType);
        void LoadShader(std::unique_ptr<Device>& device, const std::vector<char>& vShaderCode, vk::ShaderStageFlagBits fShaderType);
        void RecreateShaders(std::unique_ptr<Device>& device);
    private:
        std::vector<FShaderCache> m_vShaderCache;
        std::vector<vk::PipelineShaderStageCreateInfo> m_vShaderBuffer;
        FPipelineCreateInfo savedInfo;

        FPipeline data;
    };
};