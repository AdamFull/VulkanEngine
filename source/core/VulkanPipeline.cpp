#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "filesystem/FilesystemHelper.h"

namespace Engine
{
    PipelineBase::PipelineBase()
    {

    }

    PipelineBase::~PipelineBase()
    {

    }

    void PipelineBase::Create(FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, vk::DescriptorSetLayout& descriptorSetLayout)
    {
        savedInfo = std::move(createInfo);
        CreatePipelineLayout(device, descriptorSetLayout);
        CreatePipeline(device, swapchain);
    }

    FPipelineCreateInfo PipelineBase::CreatePipelineConfig(uint32_t width, uint32_t height, vk::PrimitiveTopology topology, 
                                                           vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, vk::FrontFace fontFace, 
                                                           vk::SampleCountFlagBits samples)
    {
        FPipelineCreateInfo createInfo{};

        createInfo.inputAssembly.topology = topology;
        createInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

        createInfo.viewport.x = 0.0f;
        createInfo.viewport.y = 0.0f;
        createInfo.viewport.width = (float)width;
        createInfo.viewport.height = (float)height;
        createInfo.viewport.minDepth = 0.0f;
        createInfo.viewport.maxDepth = 1.0f;

        createInfo.scissor.offset = vk::Offset2D{0, 0};
        createInfo.scissor.extent = vk::Extent2D{width, height};

        createInfo.rasterizer.depthClampEnable = VK_FALSE;
        createInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
        createInfo.rasterizer.polygonMode = polygonMode;
        createInfo.rasterizer.lineWidth = 1.0f;
        createInfo.rasterizer.cullMode = cullMode;
        createInfo.rasterizer.frontFace = fontFace;
        createInfo.rasterizer.depthBiasEnable = VK_FALSE;

        createInfo.multisampling.sampleShadingEnable = VK_TRUE;
        createInfo.multisampling.minSampleShading = .2f;
        createInfo.multisampling.rasterizationSamples = samples;

        createInfo.colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        createInfo.colorBlendAttachment.blendEnable = VK_FALSE;

        createInfo.colorBlending.logicOpEnable = VK_FALSE;
        createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
        createInfo.colorBlending.attachmentCount = 1;
        createInfo.colorBlending.blendConstants[0] = 0.0f;
        createInfo.colorBlending.blendConstants[1] = 0.0f;
        createInfo.colorBlending.blendConstants[2] = 0.0f;
        createInfo.colorBlending.blendConstants[3] = 0.0f;

        createInfo.depthStencil.depthTestEnable = VK_TRUE;
        createInfo.depthStencil.depthWriteEnable = VK_TRUE;
        createInfo.depthStencil.depthCompareOp = vk::CompareOp::eLess;
        createInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
        createInfo.depthStencil.stencilTestEnable = VK_FALSE;

        return createInfo;
    }

    FPipelineCreateInfo PipelineBase::CreateDefaultPipelineConfig(uint32_t width, uint32_t height, vk::SampleCountFlagBits samples)
    {
        FPipelineCreateInfo createInfo = CreatePipelineConfig(width, height, vk::PrimitiveTopology::eTriangleList, vk::PolygonMode::eFill,
                                    vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, samples);
        createInfo.bindPoint = vk::PipelineBindPoint::eGraphics;
        return createInfo;
    }

    FPipelineCreateInfo PipelineBase::CreateDefaultDebugPipelineConfig(uint32_t width, uint32_t height, vk::SampleCountFlagBits samples)
    {
        FPipelineCreateInfo createInfo = CreatePipelineConfig(width, height, vk::PrimitiveTopology::eTriangleList, vk::PolygonMode::eLine,
                                    vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, samples);
        createInfo.bindPoint = vk::PipelineBindPoint::eGraphics;
        return createInfo;
    }

    void PipelineBase::Bind(vk::CommandBuffer& commandBuffer)
    {
        commandBuffer.bindPipeline(savedInfo.bindPoint, data.pipeline);
    }

    void PipelineBase::PushConstants(vk::CommandBuffer& commandBuffer, FUniformData& ubo)
    {
        //commandBuffer.pushConstants(data.layout, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, sizeof(FUniformData), &ubo);
    }

    //TODO: In future move it to render main
    void PipelineBase::CreatePipelineLayout(std::unique_ptr<Device>& device, vk::DescriptorSetLayout& descriptorSetLayout)
    {
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        data.layout = device->Make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutInfo);
        assert(data.layout && "Pipeline layout was not created");
    }

    void PipelineBase::CreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
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
        pipelineInfo.layout = data.layout;
        pipelineInfo.renderPass = swapchain->get().renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = nullptr;

        data.pipeline = device->Make<vk::Pipeline, vk::GraphicsPipelineCreateInfo>(pipelineInfo);
        assert(data.pipeline && "Failed creating pipeline.");
    }

    void PipelineBase::LoadShader(std::unique_ptr<Device>& device, const std::string& srShaderPath, vk::ShaderStageFlagBits fShaderType)
    {
        auto shader_code = FilesystemHelper::ReadFile(srShaderPath);
        m_vShaderCache.emplace_back(FShaderCache{ fShaderType, shader_code});
        LoadShader(device, shader_code, fShaderType);
    }

    void PipelineBase::LoadShader(std::unique_ptr<Device>& device, const std::map<vk::ShaderStageFlagBits, std::string>& mShaders)
    {
        for(auto& [key, value]: mShaders)
        {
            LoadShader(device, value, key);
        }
    }

    void PipelineBase::LoadShader(std::unique_ptr<Device>& device, const std::vector<char>& vShaderCode, vk::ShaderStageFlagBits fShaderType) 
    {
        vk::ShaderModule shaderModule;

        try
        {
            shaderModule = device->Make<vk::ShaderModule, vk::ShaderModuleCreateInfo>
            (
                vk::ShaderModuleCreateInfo
                {
                    vk::ShaderModuleCreateFlags(),
                    vShaderCode.size(),
                    reinterpret_cast<const uint32_t *>(vShaderCode.data())
                }
            );
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create shader module!");
        }

        m_vShaderBuffer.emplace_back
        (
            vk::PipelineShaderStageCreateFlags(),
            fShaderType,
            shaderModule,
            "main"
        );
    }

    void PipelineBase::RecreateShaders(std::unique_ptr<Device>& device)
    {
        m_vShaderBuffer.clear();

        for(auto& cached : m_vShaderCache)
        {
            LoadShader(device, cached.srShaderData, cached.sShaderType);
        }
    }

    void PipelineBase::RecreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, vk::DescriptorSetLayout& descriptorSetLayout)
    {

        device->Destroy(data.pipeline);
        device->Destroy(data.layout);

        savedInfo.viewport.width = swapchain->get().extent.width;
        savedInfo.viewport.height = swapchain->get().extent.height;
        savedInfo.scissor.extent = swapchain->get().extent;

        RecreateShaders(device);
        CreatePipelineLayout(device, descriptorSetLayout);
        CreatePipeline(device, swapchain);
    }

    void PipelineBase::Cleanup(std::unique_ptr<Device>& device)
    {
        device->Destroy(data.pipeline);
        device->Destroy(data.layout);
    }
}