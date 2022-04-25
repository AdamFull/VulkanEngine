#pragma once
#include "Pipeline.h"

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            class ComputePipeline : public PipelineBase
            {
            public:
                ComputePipeline() = default;

                void Create(vk::PipelineBindPoint bindPoint,  const std::vector<Shader::Define>& vDefines, const std::vector<std::string>& vStages) override;
                void RecreatePipeline() override;

            protected:
                void CreatePipeline() override;
            };
        }
    }
}