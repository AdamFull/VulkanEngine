#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "filesystem/FilesystemHelper.h"

namespace Engine
{
    VulkanPipeline::VulkanPipeline(std::unique_ptr<VulkanDevice>& device)
    {
        m_Device = device->GetLogical().get();
    }

    void VulkanPipeline::CreatePipeline(const FPipelineConfigInfo& configInfo)
    {
        assert(configInfo.pipelineLayout && "Cannot create graphics pipeline: no pipelineLayout provided in config info");
        assert(configInfo.renderPass && "Cannot create graphics pipeline: no renderPass provided in config info");

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;

        vk::GraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.stageCount = m_vShaderBuffer.size();
        pipelineInfo.pStages = m_vShaderBuffer.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &configInfo.viewportInfo;
        pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
        pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
        pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
        pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
        pipelineInfo.pDynamicState = nullptr;
        pipelineInfo.layout = configInfo.pipelineLayout;
        pipelineInfo.renderPass = configInfo.renderPass;
        pipelineInfo.subpass = configInfo.subpass;
        pipelineInfo.basePipelineHandle = nullptr;
        pipelineInfo.basePipelineIndex = -1;

        try
        {
            m_GraphicsPipeline = m_Device.createGraphicsPipeline(nullptr, pipelineInfo).value;
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }

        for(auto& shader : m_vShaderBuffer)
            m_Device.destroyShaderModule(shader.module);
        
        m_vShaderBuffer.clear();
    }

    FPipelineConfigInfo VulkanPipeline::PipelineDefault(uint32_t width, uint32_t height)
    {
        FPipelineConfigInfo configInfo{};
        configInfo.inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
        configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        configInfo.viewport.x = 0.0f;
        configInfo.viewport.y = 0.0f;
        configInfo.viewport.width = static_cast<float>(width);
        configInfo.viewport.height = static_cast<float>(height);
        configInfo.viewport.minDepth = 0.0f;
        configInfo.viewport.maxDepth = 1.0f;

        configInfo.scissor.offset = vk::Offset2D{0, 0};
        configInfo.scissor.extent = vk::Extent2D{width, height};

        configInfo.viewportInfo.viewportCount = 1;
        configInfo.viewportInfo.pViewports = &configInfo.viewport;
        configInfo.viewportInfo.scissorCount = 1;
        configInfo.viewportInfo.pScissors = &configInfo.scissor;

        configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
        configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        configInfo.rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
        configInfo.rasterizationInfo.lineWidth = 1.0f;
        configInfo.rasterizationInfo.cullMode = vk::CullModeFlagBits::eNone;
        configInfo.rasterizationInfo.frontFace = vk::FrontFace::eClockwise;
        configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
        configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
        configInfo.rasterizationInfo.depthBiasClamp = 0.0f;          // Optional
        configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;    // Optional

        configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        configInfo.multisampleInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
        configInfo.multisampleInfo.minSampleShading = 1.0f;          // Optional
        configInfo.multisampleInfo.pSampleMask = nullptr;            // Optional
        configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
        configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;      // Optional

        configInfo.colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
        configInfo.colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;  // Optional
        configInfo.colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
        configInfo.colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;             // Optional
        configInfo.colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;  // Optional
        configInfo.colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
        configInfo.colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;             // Optional

        configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
        configInfo.colorBlendInfo.logicOp = vk::LogicOp::eCopy; // Optional
        configInfo.colorBlendInfo.attachmentCount = 1;
        configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
        configInfo.colorBlendInfo.blendConstants[0] = 0.0f; // Optional
        configInfo.colorBlendInfo.blendConstants[1] = 0.0f; // Optional
        configInfo.colorBlendInfo.blendConstants[2] = 0.0f; // Optional
        configInfo.colorBlendInfo.blendConstants[3] = 0.0f; // Optional

        configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
        configInfo.depthStencilInfo.depthCompareOp = vk::CompareOp::eLess;
        configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        configInfo.depthStencilInfo.minDepthBounds = 0.0f; // Optional
        configInfo.depthStencilInfo.maxDepthBounds = 1.0f; // Optional
        configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
        configInfo.depthStencilInfo.front = vk::StencilOpState{}; // Optional
        configInfo.depthStencilInfo.back = vk::StencilOpState{};  // Optional

        return configInfo;
    }

    void VulkanPipeline::LoadShader(const std::string& srShaderPath, vk::ShaderStageFlagBits fShaderType)
    {
        auto shader_code = FilesystemHelper::ReadFile(srShaderPath);
        m_vShaderCache.emplace_back(FShaderCache{ fShaderType, shader_code});
        LoadShader(shader_code, fShaderType);
    }

    void VulkanPipeline::LoadShader(const shader_load_map_t& mShaders)
    {
        for(auto& [key, value]: mShaders)
        {
            LoadShader(value, key);
        }
    }

    void VulkanPipeline::LoadShader(const std::vector<char>& vShaderCode, vk::ShaderStageFlagBits fShaderType) 
    {
        vk::ShaderModule shaderModule;

        try
        {
            shaderModule = m_Device.createShaderModule
            (
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
            srShaderEntriePoint
        );
    }

    void VulkanPipeline::RecreateShaders()
    {
        m_vShaderBuffer.clear();

        for(auto& cached : m_vShaderCache)
        {
            LoadShader(cached.srShaderData, cached.sShaderType);
        }
    }
}