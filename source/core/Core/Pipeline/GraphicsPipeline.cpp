#include "GraphicsPipeline.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanSwapChain.h"

using namespace Engine::Core::Pipeline;

GraphicsPipeline::GraphicsPipeline(std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapchain)
{
    m_device = device;
    m_swapchain = swapchain;
}

void GraphicsPipeline::Create(FPipelineCreateInfo createInfo)
{
    savedInfo = std::move(createInfo);
    CreatePipeline();
}

void GraphicsPipeline::CreatePipeline()
{
    assert(m_device && "Cannot create pipeline, cause logical device is not valid.");
    assert(m_swapchain && "Cannot create pipeline, cause render pass is not valid.");

    savedInfo.vertexInputInfo.pVertexBindingDescriptions = &savedInfo.vertexInputDesc;
    savedInfo.vertexInputInfo.pVertexAttributeDescriptions = savedInfo.vertexAtribDesc.data();

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.viewportCount = 1;
    viewportState.pViewports = &savedInfo.viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &savedInfo.scissor;

    savedInfo.colorBlending.pAttachments = &savedInfo.colorBlendAttachment;

    vk::GraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.stageCount = m_vShaderBuffer.size();
    pipelineInfo.pStages = m_vShaderBuffer.data();
    pipelineInfo.pVertexInputState = &savedInfo.vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &savedInfo.inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &savedInfo.rasterizer;
    pipelineInfo.pMultisampleState = &savedInfo.multisampling;
    pipelineInfo.pColorBlendState = &savedInfo.colorBlending;
    pipelineInfo.pDepthStencilState = &savedInfo.depthStencil;
    pipelineInfo.layout = savedInfo.pipelineLayout;
    pipelineInfo.renderPass = savedInfo.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.pDynamicState = &savedInfo.dynamicStateInfo;

    m_device->GetLogical().createGraphicsPipelines(savedInfo.pipelineCache, 1, &pipelineInfo, nullptr, &data.pipeline);
    assert(data.pipeline && "Failed creating pipeline.");
}

void GraphicsPipeline::RecreatePipeline(FPipelineCreateInfo createInfo)
{
    savedInfo = createInfo;

    RecreateShaders();
    CreatePipeline();
}