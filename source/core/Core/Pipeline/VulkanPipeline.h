#pragma once
#include "PipelineConfig.h"
#include "Shader.hpp"

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            class Shader;
            class PipelineBase
            {
            public:
                PipelineBase() = default;
                virtual ~PipelineBase();

                virtual void Create(FPipelineCreateInfo createInfo);

                void Bind(vk::CommandBuffer &commandBuffer);

                virtual void LoadShader(const std::vector<std::string> &vShaders);
                virtual void RecreatePipeline(FPipelineCreateInfo createInfo);

                virtual void Cleanup();

                inline virtual vk::Pipeline &GetPipeline() { return m_pipeline; }
                inline virtual vk::PipelineBindPoint &GetBindPoint() { return savedInfo.bindPoint; }

                inline vk::DescriptorSetLayout& GetDescriptorSetLayout() { return m_descriptorSetLayout; }
                inline vk::DescriptorPool& GetDescriptorPool() { return m_descriptorPool; }
                inline vk::PipelineLayout& GetPipelineLayout() { return m_pipelineLayout; }

                inline std::unique_ptr<Shader>& GetShader() { return m_pShader; }

            protected:
                void CreateDescriptorSetLayout();
                void CreateDescriptorPool();
                void CreatePipelineLayout();

                virtual void CreatePipeline() {}
                virtual void RecreateShaders();

                std::vector<std::string> m_vShaderCache;

                std::vector<Shader::Define> m_vDefines;

                std::unique_ptr<Shader> m_pShader;

                vk::DescriptorSetLayout m_descriptorSetLayout{nullptr};
                vk::DescriptorPool m_descriptorPool{nullptr};
                vk::PipelineLayout m_pipelineLayout{nullptr};

                FPipelineCreateInfo savedInfo;
                vk::Pipeline m_pipeline;
            };
        }
    }
};