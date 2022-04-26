#include "ComputePipeline.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core::Pipeline;

void ComputePipeline::Create(vk::PipelineBindPoint bindPoint,  const std::vector<Shader::Define>& vDefines, const std::vector<std::string>& vStages)
{
    PipelineBase::Create(bindPoint, vDefines, vStages);
    CreatePipeline();
}

void ComputePipeline::RecreatePipeline()
{
    RecreateShaders();
    CreatePipeline();
}

void ComputePipeline::CreatePipeline()
{
    assert(UDevice && "Cannot create pipeline, cause logical device is not valid.");
    assert(USwapChain && "Cannot create pipeline, cause render pass is not valid.");

    auto shaderStages = m_pShader->GetStageCreateInfo();

    vk::ComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.stage = shaderStages.front();
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = -1;

    auto result = UDevice->GetLogical().createComputePipelines(UHLInstance->GetPipelineCache(), 1, &pipelineInfo, nullptr, &m_pipeline);
    assert(m_pipeline && "Failed creating pipeline.");
}