#include "GraphicsPipeline.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanSwapChain.h"

namespace Engine
{
    void GraphicsPipeline::Create(FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        savedInfo = std::move(createInfo);
        CreatePipeline(device, swapchain);
    }

    void GraphicsPipeline::CreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        assert(device && "Cannot create pipeline, cause logical device is not valid.");
        assert(swapchain && "Cannot create pipeline, cause render pass is not valid.");
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.viewportCount = 1;
        viewportState.pViewports = &savedInfo.viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &savedInfo.scissor;

        savedInfo.colorBlending.pAttachments = &savedInfo.colorBlendAttachment;

        vk::GraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.stageCount = m_vShaderBuffer.size();
        pipelineInfo.pStages = m_vShaderBuffer.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &savedInfo.inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &savedInfo.rasterizer;
        pipelineInfo.pMultisampleState = &savedInfo.multisampling;
        pipelineInfo.pColorBlendState = &savedInfo.colorBlending;
        pipelineInfo.pDepthStencilState = &savedInfo.depthStencil;
        pipelineInfo.layout = savedInfo.pipelineLayout;
        pipelineInfo.renderPass = swapchain->GetRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = nullptr;

        data.pipeline = device->Make<vk::Pipeline, vk::GraphicsPipelineCreateInfo>(pipelineInfo);
        assert(data.pipeline && "Failed creating pipeline.");
    }

    void GraphicsPipeline::RecreatePipeline(FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        savedInfo = createInfo;

        RecreateShaders(device);
        CreatePipeline(device, swapchain);
    }
}