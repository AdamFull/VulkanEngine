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

    vk::SpecializationMapEntry specializationEntry{};
    specializationEntry.constantID = 0;
	specializationEntry.offset = 0;
	specializationEntry.size = sizeof(uint32_t);

    uint32_t specializationData = static_cast<uint32_t>(UDevice->GetSamples());
	vk::SpecializationInfo specializationInfo;
	specializationInfo.mapEntryCount = 1;
	specializationInfo.pMapEntries = &specializationEntry;
	specializationInfo.dataSize = sizeof(specializationData);
	specializationInfo.pData = &specializationData;

    auto shaderStages = m_pShader->GetStageCreateInfo();
    auto foundStage = std::find_if(shaderStages.begin(), shaderStages.end(), [](const vk::PipelineShaderStageCreateInfo& ci){
        return ci.stage & vk::ShaderStageFlagBits::eFragment;
    });

    if(foundStage != shaderStages.begin())
        foundStage->pSpecializationInfo = &specializationInfo;

    vk::ComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.stage = shaderStages.front();
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = -1;

    auto result = UDevice->GetLogical().createComputePipelines(UHLInstance->GetPipelineCache(), 1, &pipelineInfo, nullptr, &m_pipeline);
    assert(m_pipeline && "Failed creating pipeline.");
}