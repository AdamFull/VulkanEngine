#pragma once
#include <functional>
#include "PipelineConfig.h"

namespace Engine
{
    namespace Core
    {
        class Device;
        class SwapChain;
        namespace Pipeline
        {
            class PipelineBase;
            /*struct FPipelineComp
    {
        static EPipelineType GetType(std::string srType);
        static std::map<std::string, EPipelineType> m_mComp;
    };*/

            struct PipelineFactory
            {
                using signature = std::unique_ptr<PipelineBase>(FPipelineCreateInfo);
                static std::unique_ptr<PipelineBase> CreatePipeline(FPipelineCreateInfo createInfo);
                static std::map<EPipelineType, std::function<signature>> m_mFactory;

                static std::map<EShaderSet, std::map<vk::ShaderStageFlagBits, std::string>> mShaderSets;
            };
        }
    }
}