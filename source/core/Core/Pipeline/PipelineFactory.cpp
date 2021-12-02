#include "PipelineFactory.h"
#include "GraphicsPipeline.h"
#include "Core/VulkanHighLevel.h"

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
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/compiled/prefilterenvmap/frag.spv"}}},
    {EShaderSet::eDeferred,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/compiled/deferred/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/compiled/deferred/frag.spv"}}},
      
    {EShaderSet::eShadow,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/compiled/shadow/vert.spv"},
      {vk::ShaderStageFlagBits::eGeometry, "../../assets/shaders/compiled/shadow/geom.spv"}}},

    {EShaderSet::eNormalDebugging,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/compiled/normalDraw/vert.spv"},
      {vk::ShaderStageFlagBits::eGeometry, "../../assets/shaders/compiled/normalDraw/geom.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/compiled/normalDraw/frag.spv"}}}}; 

std::map<EPipelineType, std::function<PipelineFactory::signature>> PipelineFactory::m_mFactory =
    {
        {EPipelineType::eGraphics,
            [](FPipelineCreateInfo createInfo)
            {
               auto pipeline = std::make_unique<GraphicsPipeline>();
               pipeline->LoadShader(mShaderSets[createInfo.eSet]);
               pipeline->Create(createInfo);
               return pipeline;
            }
        }};

std::unique_ptr<PipelineBase> PipelineFactory::CreatePipeline(FPipelineCreateInfo createInfo)
{
    return m_mFactory[createInfo.eType](createInfo);
}