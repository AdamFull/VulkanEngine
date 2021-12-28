#pragma once
#include <functional>
#include "PipelineConfig.h"

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            class PipelineBase;

            struct PipelineFactory
            {
                using signature = std::unique_ptr<PipelineBase>(FPipelineCreateInfo);
                static std::unique_ptr<PipelineBase> CreatePipeline(FPipelineCreateInfo createInfo);
                static std::map<vk::PipelineBindPoint, std::function<signature>> m_mFactory;
            };
        }
    }
}