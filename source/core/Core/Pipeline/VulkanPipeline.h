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

                virtual void Bind(vk::CommandBuffer &commandBuffer);

                virtual void LoadShader(const std::vector<std::string> &vShaders);
                virtual void RecreatePipeline(FPipelineCreateInfo createInfo);

                virtual void Cleanup();

                inline virtual vk::Pipeline &GetPipeline() { return m_pipeline; }
                inline virtual vk::PipelineBindPoint &GetBindPoint() { return savedInfo.bindPoint; }

            protected:
                virtual void CreatePipeline() {}
                virtual void RecreateShaders();

                std::vector<std::string> m_vShaderCache;

                std::vector<Shader::Define> m_vDefines;

                std::unique_ptr<Shader> m_pShader;
                FPipelineCreateInfo savedInfo;
                vk::Pipeline m_pipeline;
            };
        }
    }
};