#include "PipelineFactory.h"
#include "GraphicsPipeline.h"
#include "Core/VulkanAllocator.h"

using namespace Engine::Core;
using namespace Engine::Core::Pipeline;

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
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/compiled/brdflut_gen/frag.spv"}}},
    {EShaderSet::eIrradiateCube,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/compiled/filtercube/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/compiled/irradiancecube/frag.spv"}}},
    {EShaderSet::ePrefiltred,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/compiled/filtercube/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/compiled/prefilterenvmap/frag.spv"}}}}; 

std::map<EPipelineType, std::function<PipelineFactory::signature>> PipelineFactory::m_mFactory =
    {
        {EPipelineType::eGraphics,
            [](FPipelineCreateInfo createInfo)
            {
               auto pipeline = FDefaultAllocator::Allocate<GraphicsPipeline>();
               pipeline->LoadShader(mShaderSets[createInfo.eSet]);
               pipeline->Create(createInfo);
               return pipeline;
            }
        }};

std::unique_ptr<PipelineBase> PipelineFactory::CreatePipeline(FPipelineCreateInfo createInfo)
{
    return m_mFactory[createInfo.eType](createInfo);
}