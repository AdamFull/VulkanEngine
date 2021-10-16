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

    std::map<EShaderSet, std::map<vk::ShaderStageFlagBits, std::string>> PipelineFactory::mShaderSets
    {
        {
            EShaderSet::eUI,
            {
                {vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/ui_vert.spv"},
                {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/ui_frag.spv"}
            }
        },
        {
            EShaderSet::eDiffuse,
            {
                {vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/diffuse_vert.spv"},
                {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/diffuse_frag.spv"}
            }
        },
        {
            EShaderSet::eSkybox,
            {
                {vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/skybox/skybox_vert.spv"},
                {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/skybox/skybox_frag.spv"}
            }
        },
        {
            EShaderSet::eSkybox,
            {
                {vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/skybox/reflect_vert.spv"},
                {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/skybox/reflect_frag.spv"}
            }
        },
        {
            EShaderSet::ePBR,
            {
                {vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/pbr/pbr_base_vert.spv"},
                {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/pbr/pbr_base_frag.spv"}
            }
        }
    };

    std::map<EPipelineType, std::function<PipelineFactory::signature>> PipelineFactory::m_mFactory =
    {
        {EPipelineType::eGraphics, 
            [](FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
            {
                auto pipeline = std::make_unique<GraphicsPipeline>();
                pipeline->LoadShader(device, mShaderSets[createInfo.eSet]);
                pipeline->Create(createInfo, device, swapchain);
                return pipeline;
            }
        }
    };

    std::unique_ptr<PipelineBase> PipelineFactory::CreatePipeline(FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        return m_mFactory[createInfo.eType](createInfo, device, swapchain);
    }
}