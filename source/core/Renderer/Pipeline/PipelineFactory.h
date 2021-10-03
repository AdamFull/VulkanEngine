#pragma once
#include <functional>
#include "PipelineConfig.h"

namespace Engine
{
    class Device;
    class SwapChain;
    class PipelineBase;

    /*struct FPipelineComp
    {
        static EPipelineType GetType(std::string srType);
        static std::map<std::string, EPipelineType> m_mComp;
    };*/

    struct PipelineFactory
    {
        using signature = std::unique_ptr<PipelineBase>(FPipelineCreateInfo, std::unique_ptr<Device>&, std::unique_ptr<SwapChain>&);
        static std::unique_ptr<PipelineBase> CreatePipeline(FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
        static std::map<EPipelineType, std::function<signature>> m_mFactory;

        static std::map<EShaderSet, std::map<vk::ShaderStageFlagBits, std::string>> mShaderSets;
    };
    
}