#include "GraphicsPipeline.h"
#include "Core/VulkanHighLevel.h"
#include "Core/VulkanInitializers.h"

using namespace Engine::Core::Pipeline;

void GraphicsPipeline::Create(VertexInput&& vertexInput, vk::RenderPass& renderPass, vk::PipelineBindPoint bindPoint, uint32_t colorAttachments, vk::CullModeFlagBits culling, vk::FrontFace fontface, 
                              vk::Bool32 enableDepth, const std::vector<vk::DynamicState>& vDynamicStateEnables, const std::vector<Shader::Define>& vDefines, const std::vector<std::string>& vStages)
{
    PipelineBase::Create(std::move(vertexInput), renderPass, bindPoint, colorAttachments, culling, fontface, enableDepth, vDynamicStateEnables, vDefines, vStages);
    CreatePipeline();
}

void GraphicsPipeline::CreatePipeline()
{
    assert(UDevice && "Cannot create pipeline, cause logical device is not valid.");
    assert(USwapChain && "Cannot create pipeline, cause render pass is not valid.");

    auto& bindingDescription = m_vertexInput.GetInputBindingDescription();
    auto& attributeDescription = m_vertexInput.GetInputAttributeDescription();

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

    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;
    for(uint32_t i = 0; i < m_colorAttachments; i++)
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

    auto shaderStages = m_pShader->GetStageCreateInfo();

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
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.pDynamicState = &dynamicStateInfo;

    auto result = UDevice->GetLogical().createGraphicsPipelines(UHLInstance->GetPipelineCache(), 1, &pipelineInfo, nullptr, &m_pipeline);
    assert(m_pipeline && "Failed creating pipeline.");
}

void GraphicsPipeline::RecreatePipeline()
{
    RecreateShaders();
    CreatePipeline();
}