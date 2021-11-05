#include "PipelineFactory.h"
#include "GraphicsPipeline.h"

using namespace Engine::Core::Pipeline;
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

std::map<EShaderSet, std::map<vk::ShaderStageFlagBits, std::string>> PipelineFactory::mShaderSets{
    {EShaderSet::eUI,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/compiled/ui/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/compiled/ui/frag.spv"}}},
    {EShaderSet::eDiffuse,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/compiled/diffuse/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/compiled/diffuse/frag.spv"}}},
    {EShaderSet::eSkybox,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/compiled/skybox/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/compiled/skybox/frag.spv"}}},
    {EShaderSet::eBRDF,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/compiled/brdflut_gen/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/compiled/brdflut_gen/frag.spv"}}}};

std::map<EPipelineType, std::function<PipelineFactory::signature>> PipelineFactory::m_mFactory =
    {
        {EPipelineType::eGraphics,
         [](FPipelineCreateInfo createInfo, std::unique_ptr<Device> &device, std::unique_ptr<SwapChain> &swapchain)
         {
             auto pipeline = std::make_unique<GraphicsPipeline>();
             pipeline->LoadShader(device, mShaderSets[createInfo.eSet]);
             pipeline->Create(createInfo, device, swapchain);
             return pipeline;
         }}};

std::unique_ptr<PipelineBase> PipelineFactory::CreatePipeline(FPipelineCreateInfo createInfo, std::unique_ptr<Device> &device, std::unique_ptr<SwapChain> &swapchain)
{
    return m_mFactory[createInfo.eType](createInfo, device, swapchain);
}