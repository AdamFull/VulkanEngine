#include "PipelineFactory.h"
#include "GraphicsPipeline.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Pipeline;

/*std::map<EShaderSet, std::map<vk::ShaderStageFlagBits, std::string>> PipelineFactory::mShaderSets{
    {EShaderSet::eUI,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/main/ui/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/main/ui/frag.spv"}}},
    {EShaderSet::eDiffuse,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/main/diffuse/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/main/diffuse/frag.spv"}}},
    {EShaderSet::eSkybox,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/main/skybox/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/main/skybox/frag.spv"}}},
      
    {EShaderSet::eBRDF,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/main/screenspace/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/generators/brdflut_gen/frag.spv"}}},
    {EShaderSet::eIrradiateCube,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/generators/filtercube/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/generators/irradiancecube/frag.spv"}}},
    {EShaderSet::ePrefiltred,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/generators/filtercube/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/generators/prefilterenvmap/frag.spv"}}},
    {EShaderSet::eDeferred,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/main/screenspace/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/main/deferred/frag.spv"}}},
    {EShaderSet::ePostProcess,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/main/screenspace/vert.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/postprocess/bloom/frag.spv"}}},
      
    {EShaderSet::eShadow,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/main/shadow/vert.spv"},
      {vk::ShaderStageFlagBits::eGeometry, "../../assets/shaders/main/shadow/geom.spv"}}},

    {EShaderSet::eNormalDebugging,
     {{vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/debug/normalDraw/vert.spv"},
      {vk::ShaderStageFlagBits::eGeometry, "../../assets/shaders/debug/normalDraw/geom.spv"},
      {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/debug/normalDraw/frag.spv"}}}}; */

std::map<vk::PipelineBindPoint, std::function<PipelineFactory::signature>> PipelineFactory::m_mFactory =
    {
        {vk::PipelineBindPoint::eGraphics,
            [](FPipelineCreateInfo createInfo)
            {
               auto pipeline = std::make_unique<GraphicsPipeline>();
               pipeline->LoadShader(createInfo.shaders);
               pipeline->Create(createInfo);
               return pipeline;
            }
        }};

std::unique_ptr<PipelineBase> PipelineFactory::CreatePipeline(FPipelineCreateInfo createInfo)
{
    return m_mFactory[createInfo.bindPoint](createInfo);
}