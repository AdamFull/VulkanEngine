#include "ComputePipeline.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core::Pipeline;

void CComputePipeline::create()
{
    CPipelineBase::create();
    createPipeline();
}

void CComputePipeline::createPipeline()
{
    assert(CDevice::inst() && "Cannot create pipeline, cause logical device is not valid.");
    assert(CSwapChain::inst() && "Cannot create pipeline, cause render pass is not valid.");

    auto shaderStages = m_pShader->getStageCreateInfo();

    vk::ComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.stage = shaderStages.front();
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = -1;

    auto result = CDevice::inst()->getLogical().createComputePipelines(UHLInstance->getPipelineCache(), 1, &pipelineInfo, nullptr, &m_pipeline);
    assert(m_pipeline && "Failed creating pipeline.");
}