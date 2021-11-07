#pragma once
#include "PipelineConfig.h"

namespace Engine
{
    namespace Core
    {
        class Device;
        class SwapChain;
        namespace Pipeline
        {
            struct FShaderCache
            {
                vk::ShaderStageFlagBits sShaderType;
                std::vector<char> srShaderData;
            };

            struct FPipeline
            {
                vk::Pipeline pipeline;
            };

            class PipelineBase
            {
            public:
                PipelineBase() = default;
                PipelineBase(std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapchain);
                virtual ~PipelineBase();

                virtual void Create(FPipelineCreateInfo createInfo);

                virtual void Bind(vk::CommandBuffer &commandBuffer);

                virtual void LoadShader(const std::map<vk::ShaderStageFlagBits, std::string> &mShaders);
                virtual void RecreatePipeline(FPipelineCreateInfo createInfo);

                virtual void Cleanup();

                inline virtual vk::Pipeline &GetPipeline() { return data.pipeline; }
                inline virtual vk::PipelineBindPoint &GetBindPoint() { return savedInfo.bindPoint; }
                inline virtual FPipeline &get() { return data; }

            protected:
                virtual void CreatePipeline() {}

                virtual void LoadShader(const std::string &srShaderPath, vk::ShaderStageFlagBits fShaderType);
                virtual void LoadShader(const std::vector<char> &vShaderCode, vk::ShaderStageFlagBits fShaderType);
                virtual void RecreateShaders();
                virtual void DestroyShaders();

                std::vector<FShaderCache> m_vShaderCache;
                std::vector<vk::PipelineShaderStageCreateInfo> m_vShaderBuffer;
                FPipelineCreateInfo savedInfo;

                std::shared_ptr<Device> m_device;
                std::shared_ptr<SwapChain> m_swapchain;

                FPipeline data;
            };
        }
    }
};