#include "GraphicsPipeline.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core::Pipeline;

void GraphicsPipeline::Create(FPipelineCreateInfo createInfo)
{
    PipelineBase::Create(createInfo);
    CreatePipeline();
}

void GraphicsPipeline::CreatePipeline()
{
    assert(UDevice && "Cannot create pipeline, cause logical device is not valid.");
    assert(USwapChain && "Cannot create pipeline, cause render pass is not valid.");

    auto& bindingDescription = m_pShader->GetBindingDescription();
    auto& attributeDescription = m_pShader->GetAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputCI{};
    vertexInputCI.vertexBindingDescriptionCount = 0;
    vertexInputCI.vertexAttributeDescriptionCount = 0;
    vertexInputCI.vertexBindingDescriptionCount = attributeDescription.size() > 0 ? 1 : 0;
    vertexInputCI.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
    vertexInputCI.pVertexBindingDescriptions = &bindingDescription;
    vertexInputCI.pVertexAttributeDescriptions = attributeDescription.data();

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    savedInfo.colorBlending.pAttachments = savedInfo.colorBlendAttachments.data();

    auto shaderStages = m_pShader->GetStageCreateInfo();

    vk::GraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputCI;
    pipelineInfo.pInputAssemblyState = &savedInfo.inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &savedInfo.rasterizer;
    pipelineInfo.pMultisampleState = &savedInfo.multisampling;
    pipelineInfo.pColorBlendState = &savedInfo.colorBlending;
    pipelineInfo.pDepthStencilState = &savedInfo.depthStencil;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = savedInfo.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.pDynamicState = &savedInfo.dynamicStateInfo;

    UDevice->GetLogical().createGraphicsPipelines(UHLInstance->GetPipelineCache(), 1, &pipelineInfo, nullptr, &m_pipeline);
    assert(m_pipeline && "Failed creating pipeline.");
}

void GraphicsPipeline::RecreatePipeline(FPipelineCreateInfo createInfo)
{
    savedInfo = createInfo;

    RecreateShaders();
    CreatePipeline();
}