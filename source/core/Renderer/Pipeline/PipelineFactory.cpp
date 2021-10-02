#include "PipelineFactory.h"
#include "GraphicsPipeline.h"

namespace Engine
{
    /*std::map<std::string, EPipelineType> FPipelineComp::m_mComp
    {

    };

    EPipelineType FPipelineComp::GetType(std::string srType)
    {
        auto it = m_mComp.find(srType);
        if(it != m_mComp.end())
            return it->second;
        return EPipelineType::eNone;
    }*/

    std::map<vk::ShaderStageFlagBits, std::string> PipelineFactory::vDiffuse
    {
        {vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/vert.spv"},
        {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/frag.spv"}
    };

    std::map<EPipelineType, std::function<PipelineFactory::signature>> PipelineFactory::m_mFactory =
    {
        {EPipelineType::eGraphics, 
            [](FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
            {
                auto pipeline = std::make_unique<GraphicsPipeline>();
                pipeline->LoadShader(device, vDiffuse);
                pipeline->Create(createInfo, device, swapchain);
                return pipeline;
            }
        }
    };

    std::unique_ptr<PipelineBase> PipelineFactory::CreatePipeline(FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, EPipelineType eType)
    {
        return m_mFactory[eType](createInfo, device, swapchain);
    }
}