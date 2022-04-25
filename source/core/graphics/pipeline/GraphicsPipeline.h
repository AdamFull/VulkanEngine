#pragma once
#include "Pipeline.h"

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            class GraphicsPipeline : public PipelineBase
            {
            public:
                GraphicsPipeline() = default;

                void Create(VertexInput&& vertexInput, vk::RenderPass& renderPass, vk::PipelineBindPoint bindPoint, uint32_t colorAttachments, vk::CullModeFlagBits culling, vk::FrontFace fontface, 
                vk::Bool32 enableDepth, const std::vector<vk::DynamicState>& vDynamicStateEnables, const std::vector<Shader::Define>& vDefines, const std::vector<std::string>& vStages) override;
                void RecreatePipeline() override;

            protected:
                void CreatePipeline() override;
            };
        }
    }
}