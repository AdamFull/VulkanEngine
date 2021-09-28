#include "GraphicsPipelineDiffuse.h"
#include "core/VulkanDevice.h"
#include "core/VulkanSwapChain.h"

namespace Engine
{
    void GraphicsPipelineDiffuse::Create(FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::shared_ptr<SwapChain>)
    {
        savedInfo = std::move(createInfo);
        CreateDescriptorSetLayout(device);
        CreateDescriptorPool(device);
        CreateDescriptorSets(device);
        CreatePipelineLayout(device);
        CreatePipeline(device);
    }

    void GraphicsPipelineDiffuse::CreateDescriptorSetLayout(std::unique_ptr<Device>& device)
    {
        vk::DescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

        vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
        vk::DescriptorSetLayoutCreateInfo createInfo{};
        createInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
        createInfo.pBindings = bindings.data();

        //TODO: check result
        auto result = device->GetLogical()->createDescriptorSetLayout(&createInfo, nullptr, &data.descriptorSetLayout);
    }

    void GraphicsPipelineDiffuse::CreateDescriptorPool(std::unique_ptr<Device>& device)
    {
        uint32_t images = m_pSwapChain->GetImages().size();

        std::array<vk::DescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = images;
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = images;

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = images;

        //TODO:: add result checking
        data.descriptorPool = device->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(poolInfo);
    }

    void GraphicsPipelineDiffuse::CreateDescriptorSets(std::unique_ptr<Device>& device)
    {
        uint32_t images = m_pSwapChain->GetImages().size();
        std::vector<vk::DescriptorSetLayout> vDescriptorSetLayouts(images, data.descriptorSetLayout);

        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = data.descriptorPool;
        allocInfo.descriptorSetCount = images;
        allocInfo.pSetLayouts = vDescriptorSetLayouts.data();

        data.vDescriptorSets.resize(images);

        auto result = device->GetLogical()->allocateDescriptorSets(&allocInfo, data.vDescriptorSets.data());
    }

    //TODO: In future move it to render main
    void GraphicsPipelineDiffuse::CreatePipelineLayout(std::unique_ptr<Device>& device)
    {
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &data.descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        data.layout = device->Make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutInfo);
        assert(data.layout && "Pipeline layout was not created");
    }

    void GraphicsPipelineDiffuse::CreatePipeline(std::unique_ptr<Device>& device)
    {
        assert(device && "Cannot create pipeline, cause logical device is not valid.");
        assert(m_pSwapChain && "Cannot create pipeline, cause render pass is not valid.");
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
        pipelineInfo.layout = data.layout;
        pipelineInfo.renderPass = m_pSwapChain->GetRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = nullptr;

        data.pipeline = device->Make<vk::Pipeline, vk::GraphicsPipelineCreateInfo>(pipelineInfo);
        assert(data.pipeline && "Failed creating pipeline.");
    }

    void GraphicsPipelineDiffuse::RecreatePipeline(std::unique_ptr<Device>& device)
    {
        auto extent = m_pSwapChain->GetExtent();
        device->Destroy(data.pipeline);
        device->Destroy(data.layout);

        savedInfo.viewport.width = extent.width;
        savedInfo.viewport.height = extent.height;
        savedInfo.scissor.extent = extent;

        RecreateShaders(device);
        CreatePipelineLayout(device);
        CreatePipeline(device);
    }

    void GraphicsPipelineDiffuse::Cleanup(std::unique_ptr<Device>& device)
    {
        device->Destroy(data.pipeline);
        device->Destroy(data.layout);
    }
}