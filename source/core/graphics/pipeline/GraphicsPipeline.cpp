#include "GraphicsPipeline.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/VulkanInitializers.h"

using namespace Engine::Core::Pipeline;

void CGraphicsPipeline::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    CPipelineBase::create(renderPass, subpass);
    createPipeline();
}

void CGraphicsPipeline::createPipeline()
{
    assert(UDevice && "Cannot create pipeline, cause logical device is not valid.");
    assert(USwapChain && "Cannot create pipeline, cause render pass is not valid.");

    auto& bindingDescription = m_vertexInput.getInputBindingDescription();
    auto& attributeDescription = m_vertexInput.getInputAttributeDescription();

    vk::PipelineVertexInputStateCreateInfo vertexInputCI{};
    vertexInputCI.vertexBindingDescriptionCount = 0;
    vertexInputCI.vertexAttributeDescriptionCount = 0;
    vertexInputCI.vertexBindingDescriptionCount = attributeDescription.size() > 0 ? 1 : 0;
    vertexInputCI.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
    vertexInputCI.pVertexBindingDescriptions = &bindingDescription;
    vertexInputCI.pVertexAttributeDescriptions = attributeDescription.data();

    auto inputAssembly = Initializers::PipelineInputAssemblyStateCI(vk::PrimitiveTopology::eTriangleList, VK_FALSE);
    auto rasterizer = Initializers::PipelineRasterizerStateCI(vk::PolygonMode::eFill, m_culling, m_fontface);
    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::SpecializationMapEntry specializationEntry{};
    specializationEntry.constantID = 0;
	specializationEntry.offset = 0;
	specializationEntry.size = sizeof(uint32_t);

    uint32_t specializationData = static_cast<uint32_t>(UDevice->getSamples());
	vk::SpecializationInfo specializationInfo;
	specializationInfo.mapEntryCount = 1;
	specializationInfo.pMapEntries = &specializationEntry;
	specializationInfo.dataSize = sizeof(specializationData);
	specializationInfo.pData = &specializationData;

    
    auto attachmentCount = URenderer->getCurrentStage()->getRenderPass()->getCurrentDescription().colorAttachmentCount;
    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;
    for(uint32_t i = 0; i < attachmentCount; i++)
    {
        vk::PipelineColorBlendAttachmentState colorBlendAttachment =
        Initializers::PipelineColorBlendAttachmentState(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA, VK_TRUE);
        colorBlendAttachments.emplace_back(colorBlendAttachment);
    }

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
    colorBlending.pAttachments = colorBlendAttachments.data();

    auto depthStencil = Initializers::PipelineDepthStencilStateCI(m_enableDepth, m_enableDepth, vk::CompareOp::eLessOrEqual);

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.pDynamicStates = m_vDynamicStateEnables.data();
    dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(m_vDynamicStateEnables.size());
    dynamicStateInfo.flags = vk::PipelineDynamicStateCreateFlags{};

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    auto shaderStages = m_pShader->getStageCreateInfo();
    auto foundStage = std::find_if(shaderStages.begin(), shaderStages.end(), [](const vk::PipelineShaderStageCreateInfo& ci){
        return ci.stage & vk::ShaderStageFlagBits::eFragment;
    });

    if(foundStage != shaderStages.begin())
        foundStage->pSpecializationInfo = &specializationInfo;

    vk::GraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputCI;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = subpass;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.pDynamicState = &dynamicStateInfo;

    auto result = UDevice->getLogical().createGraphicsPipelines(UHLInstance->getPipelineCache(), 1, &pipelineInfo, nullptr, &m_pipeline);
    assert(m_pipeline && "Failed creating pipeline.");
}

void CGraphicsPipeline::recreatePipeline()
{
    recreateShaders();
    createPipeline();
}