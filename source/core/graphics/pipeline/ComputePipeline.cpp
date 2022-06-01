#include "ComputePipeline.h"
#include "graphics/VulkanHighLevel.h"

using namespace engine::core::pipeline;

void CComputePipeline::create()
{
    CPipelineBase::create();
    createPipeline();
}

void CComputePipeline::createPipeline()
{
    auto shaderStages = m_pShader->getStageCreateInfo();

    vk::ComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.stage = shaderStages.front();
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = -1;

    vk::Result res = CDevice::inst()->create(pipelineInfo, &pipeline);
    assert(res == vk::Result::eSuccess && "Failed creating pipeline.");
}