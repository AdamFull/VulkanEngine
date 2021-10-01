#pragma once
#include <functional>

namespace Engine
{
    class Device;
    class SwapChain;
    class PipelineBase;

    enum class EPipelineType
    {
        eNone = -1,
        eGraphicsDiffuse
    };

    /*struct FPipelineComp
    {
        static EPipelineType GetType(std::string srType);
        static std::map<std::string, EPipelineType> m_mComp;
    };*/

    struct PipelineFactory
    {
        using signature = std::unique_ptr<PipelineBase>(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
        static std::unique_ptr<PipelineBase> CreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, EPipelineType eType);
        static std::map<EPipelineType, std::function<signature>> m_mFactory;

        static std::map<vk::ShaderStageFlagBits, std::string> vDiffuse;
    };
    
}