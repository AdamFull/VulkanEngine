#include "PipelineConfig.h"
#include "Core/Window/WindowHandle.h"
#include "Core/DataTypes/VulkanVertex.hpp"

using namespace Engine::Core::Pipeline;
using namespace Engine::Core::Window;

FPipelineCreateInfo PipelineConfig::CreateUIPipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};
    createInfo.vertexInputDesc = VertexUI::getBindingDescription();
    createInfo.vertexAtribDesc = VertexUI::getAttributeDescriptions();

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 1;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.vertexAtribDesc.size());

    createInfo.inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    createInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

    createInfo.viewport.x = 0.0f;
    createInfo.viewport.y = 0.0f;
    createInfo.viewport.width = (float)WindowHandle::m_iWidth;
    createInfo.viewport.height = (float)WindowHandle::m_iHeight;
    createInfo.viewport.minDepth = 0.0f;
    createInfo.viewport.maxDepth = 1.0f;

    createInfo.scissor.offset = vk::Offset2D{0, 0};
    createInfo.scissor.extent = vk::Extent2D{static_cast<uint32_t>(WindowHandle::m_iWidth), static_cast<uint32_t>(WindowHandle::m_iHeight)};

    createInfo.rasterizer.depthClampEnable = VK_FALSE;
    createInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
    createInfo.rasterizer.polygonMode = vk::PolygonMode::eFill;
    createInfo.rasterizer.lineWidth = 1.0f;
    createInfo.rasterizer.cullMode = vk::CullModeFlagBits::eNone;
    createInfo.rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
    createInfo.rasterizer.depthBiasEnable = VK_FALSE;

    createInfo.multisampling.rasterizationSamples = samples;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
    createInfo.colorBlendAttachments.push_back(colorBlendAttachment);
    
    createInfo.colorBlending.logicOpEnable = VK_FALSE;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());
    createInfo.colorBlending.blendConstants[0] = 0.0f;
    createInfo.colorBlending.blendConstants[1] = 0.0f;
    createInfo.colorBlending.blendConstants[2] = 0.0f;
    createInfo.colorBlending.blendConstants[3] = 0.0f;

    createInfo.depthStencil.depthTestEnable = VK_TRUE;
    createInfo.depthStencil.depthWriteEnable = VK_TRUE;
    createInfo.depthStencil.depthCompareOp = vk::CompareOp::eLessOrEqual;
    createInfo.depthStencil.back.compareOp = vk::CompareOp::eAlways;
    createInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
    createInfo.depthStencil.stencilTestEnable = VK_FALSE;

    createInfo.dynamicStateEnables = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    createInfo.dynamicStateInfo.pDynamicStates = createInfo.dynamicStateEnables.data();
    createInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(createInfo.dynamicStateEnables.size());
    createInfo.dynamicStateInfo.flags = vk::PipelineDynamicStateCreateFlags{};

    createInfo.pipelineLayout = pipelineLayout;
    createInfo.pipelineCache = pipelineCache;
    createInfo.renderPass = renderPass;

    createInfo.eType = EPipelineType::eGraphics;
    createInfo.eSet = EShaderSet::eUI;

    createInfo.bindPoint = vk::PipelineBindPoint::eGraphics;

    return createInfo;
}

FPipelineCreateInfo PipelineConfig::CreateDiffusePipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};
    createInfo.vertexInputDesc = Vertex::getBindingDescription();
    createInfo.vertexAtribDesc = Vertex::getAttributeDescriptions();

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 1;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.vertexAtribDesc.size());

    createInfo.inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    createInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

    createInfo.viewport.x = 0.0f;
    createInfo.viewport.y = 0.0f;
    createInfo.viewport.width = (float)WindowHandle::m_iWidth;
    createInfo.viewport.height = (float)WindowHandle::m_iHeight;
    createInfo.viewport.minDepth = 0.0f;
    createInfo.viewport.maxDepth = 1.0f;

    createInfo.scissor.offset = vk::Offset2D{0, 0};
    createInfo.scissor.extent = vk::Extent2D{static_cast<uint32_t>(WindowHandle::m_iWidth), static_cast<uint32_t>(WindowHandle::m_iHeight)};

    createInfo.rasterizer.depthClampEnable = VK_FALSE;
    createInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
    createInfo.rasterizer.polygonMode = vk::PolygonMode::eFill;
    createInfo.rasterizer.lineWidth = 1.0f;
    createInfo.rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    createInfo.rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
    createInfo.rasterizer.depthBiasEnable = VK_FALSE;

    createInfo.multisampling.rasterizationSamples = samples;

    for(uint32_t i = 0; i < 3; i++)
    {
        vk::PipelineColorBlendAttachmentState colorBlendAttachment;
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
        colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
        createInfo.colorBlendAttachments.emplace_back(colorBlendAttachment);
    }

    createInfo.colorBlending.logicOpEnable = VK_FALSE;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());
    createInfo.colorBlending.blendConstants[0] = 0.0f;
    createInfo.colorBlending.blendConstants[1] = 0.0f;
    createInfo.colorBlending.blendConstants[2] = 0.0f;
    createInfo.colorBlending.blendConstants[3] = 0.0f;

    createInfo.depthStencil.depthTestEnable = VK_TRUE;
    createInfo.depthStencil.depthWriteEnable = VK_TRUE;
    createInfo.depthStencil.depthCompareOp = vk::CompareOp::eLessOrEqual;
    createInfo.depthStencil.back.compareOp = vk::CompareOp::eAlways;
    createInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
    createInfo.depthStencil.stencilTestEnable = VK_FALSE;

    createInfo.dynamicStateEnables = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    createInfo.dynamicStateInfo.pDynamicStates = createInfo.dynamicStateEnables.data();
    createInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(createInfo.dynamicStateEnables.size());
    createInfo.dynamicStateInfo.flags = vk::PipelineDynamicStateCreateFlags{};

    createInfo.pipelineLayout = pipelineLayout;
    createInfo.pipelineCache = pipelineCache;
    createInfo.renderPass = renderPass;

    createInfo.eType = EPipelineType::eGraphics;
    createInfo.eSet = EShaderSet::eDiffuse;

    createInfo.bindPoint = vk::PipelineBindPoint::eGraphics;

    return createInfo;
}

FPipelineCreateInfo PipelineConfig::CreateSkyboxPipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};
    createInfo.vertexInputDesc = Vertex::getBindingDescription();
    createInfo.vertexAtribDesc = Vertex::getAttributeDescriptions();

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 1;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.vertexAtribDesc.size());

    createInfo.inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    createInfo.inputAssembly.flags = vk::PipelineInputAssemblyStateCreateFlags{};
    createInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

    createInfo.rasterizer.depthClampEnable = VK_FALSE;
    createInfo.rasterizer.polygonMode = vk::PolygonMode::eFill;
    createInfo.rasterizer.lineWidth = 1.0f;
    createInfo.rasterizer.cullMode = vk::CullModeFlagBits::eFront;
    createInfo.rasterizer.frontFace = vk::FrontFace::eCounterClockwise;

    createInfo.multisampling.rasterizationSamples = samples;

    for(uint32_t i = 0; i < 3; i++)
    {
        vk::PipelineColorBlendAttachmentState colorBlendAttachment;
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;
        createInfo.colorBlendAttachments.emplace_back(colorBlendAttachment);
    }

    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());

    createInfo.depthStencil.depthTestEnable = VK_FALSE;
    createInfo.depthStencil.depthWriteEnable = VK_FALSE;
    createInfo.depthStencil.depthCompareOp = vk::CompareOp::eLessOrEqual;
    createInfo.depthStencil.back.compareOp = vk::CompareOp::eAlways;

    createInfo.viewport.x = 0.0f;
    createInfo.viewport.y = 0.0f;
    createInfo.viewport.width = (float)WindowHandle::m_iWidth;
    createInfo.viewport.height = (float)WindowHandle::m_iHeight;
    createInfo.viewport.minDepth = 0.0f;
    createInfo.viewport.maxDepth = 1.0f;

    createInfo.scissor.offset = vk::Offset2D{0, 0};
    createInfo.scissor.extent = vk::Extent2D{static_cast<uint32_t>(WindowHandle::m_iWidth), static_cast<uint32_t>(WindowHandle::m_iHeight)};

    createInfo.dynamicStateEnables = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    createInfo.dynamicStateInfo.pDynamicStates = createInfo.dynamicStateEnables.data();
    createInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(createInfo.dynamicStateEnables.size());
    createInfo.dynamicStateInfo.flags = vk::PipelineDynamicStateCreateFlags{};

    createInfo.pipelineLayout = pipelineLayout;
    createInfo.pipelineCache = pipelineCache;
    createInfo.renderPass = renderPass;

    createInfo.eType = EPipelineType::eGraphics;
    createInfo.eSet = EShaderSet::eSkybox;

    createInfo.bindPoint = vk::PipelineBindPoint::eGraphics;

    return createInfo;
}

FPipelineCreateInfo PipelineConfig::CreateBRDFPipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};
    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    createInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

    createInfo.viewport.x = 0.0f;
    createInfo.viewport.y = 0.0f;
    createInfo.viewport.width = (float)WindowHandle::m_iWidth;
    createInfo.viewport.height = (float)WindowHandle::m_iHeight;
    createInfo.viewport.minDepth = 0.0f;
    createInfo.viewport.maxDepth = 1.0f;

    createInfo.scissor.offset = vk::Offset2D{0, 0};
    createInfo.scissor.extent = vk::Extent2D{static_cast<uint32_t>(WindowHandle::m_iWidth), static_cast<uint32_t>(WindowHandle::m_iHeight)};

    createInfo.rasterizer.depthClampEnable = VK_FALSE;
    createInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
    createInfo.rasterizer.polygonMode = vk::PolygonMode::eFill;
    createInfo.rasterizer.lineWidth = 1.0f;
    createInfo.rasterizer.cullMode = vk::CullModeFlagBits::eNone;
    createInfo.rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
    createInfo.rasterizer.depthBiasEnable = VK_FALSE;

    createInfo.multisampling.rasterizationSamples = samples;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

    createInfo.colorBlendAttachments.emplace_back(colorBlendAttachment);

    createInfo.colorBlending.logicOpEnable = VK_FALSE;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());
    createInfo.colorBlending.blendConstants[0] = 0.0f;
    createInfo.colorBlending.blendConstants[1] = 0.0f;
    createInfo.colorBlending.blendConstants[2] = 0.0f;
    createInfo.colorBlending.blendConstants[3] = 0.0f;

    createInfo.depthStencil.depthTestEnable = VK_FALSE;
    createInfo.depthStencil.depthWriteEnable = VK_FALSE;
    createInfo.depthStencil.depthCompareOp = vk::CompareOp::eLessOrEqual;
    createInfo.depthStencil.back.compareOp = vk::CompareOp::eAlways;
    createInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
    createInfo.depthStencil.stencilTestEnable = VK_FALSE;

    createInfo.dynamicStateEnables = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    createInfo.dynamicStateInfo.pDynamicStates = createInfo.dynamicStateEnables.data();
    createInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(createInfo.dynamicStateEnables.size());
    createInfo.dynamicStateInfo.flags = vk::PipelineDynamicStateCreateFlags{};

    createInfo.pipelineLayout = pipelineLayout;
    createInfo.pipelineCache = pipelineCache;
    createInfo.renderPass = renderPass;

    createInfo.eType = EPipelineType::eGraphics;
    createInfo.eSet = EShaderSet::eBRDF;

    createInfo.bindPoint = vk::PipelineBindPoint::eGraphics;

    return createInfo;
}

FPipelineCreateInfo PipelineConfig::CreateIrradiatePipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};
    createInfo.vertexInputDesc = Vertex::getBindingDescription();
    createInfo.vertexAtribDesc = Vertex::getAttributeDescriptions();

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 1;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.vertexAtribDesc.size());

    createInfo.inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    createInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

    createInfo.viewport.x = 0.0f;
    createInfo.viewport.y = 0.0f;
    createInfo.viewport.width = (float)WindowHandle::m_iWidth;
    createInfo.viewport.height = (float)WindowHandle::m_iHeight;
    createInfo.viewport.minDepth = 0.0f;
    createInfo.viewport.maxDepth = 1.0f;

    createInfo.scissor.offset = vk::Offset2D{0, 0};
    createInfo.scissor.extent = vk::Extent2D{static_cast<uint32_t>(WindowHandle::m_iWidth), static_cast<uint32_t>(WindowHandle::m_iHeight)};

    createInfo.rasterizer.depthClampEnable = VK_FALSE;
    createInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
    createInfo.rasterizer.polygonMode = vk::PolygonMode::eFill;
    createInfo.rasterizer.lineWidth = 1.0f;
    createInfo.rasterizer.cullMode = vk::CullModeFlagBits::eNone;
    createInfo.rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
    createInfo.rasterizer.depthBiasEnable = VK_FALSE;

    createInfo.multisampling.rasterizationSamples = samples;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

    createInfo.colorBlendAttachments.emplace_back(colorBlendAttachment);

    createInfo.colorBlending.logicOpEnable = VK_FALSE;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());
    createInfo.colorBlending.blendConstants[0] = 0.0f;
    createInfo.colorBlending.blendConstants[1] = 0.0f;
    createInfo.colorBlending.blendConstants[2] = 0.0f;
    createInfo.colorBlending.blendConstants[3] = 0.0f;

    createInfo.depthStencil.depthTestEnable = VK_FALSE;
    createInfo.depthStencil.depthWriteEnable = VK_FALSE;
    createInfo.depthStencil.depthCompareOp = vk::CompareOp::eLessOrEqual;
    createInfo.depthStencil.back.compareOp = vk::CompareOp::eAlways;
    createInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
    createInfo.depthStencil.stencilTestEnable = VK_FALSE;

    createInfo.dynamicStateEnables = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    createInfo.dynamicStateInfo.pDynamicStates = createInfo.dynamicStateEnables.data();
    createInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(createInfo.dynamicStateEnables.size());
    createInfo.dynamicStateInfo.flags = vk::PipelineDynamicStateCreateFlags{};

    createInfo.pipelineLayout = pipelineLayout;
    createInfo.pipelineCache = pipelineCache;
    createInfo.renderPass = renderPass;

    createInfo.eType = EPipelineType::eGraphics;
    createInfo.eSet = EShaderSet::eIrradiateCube;

    createInfo.bindPoint = vk::PipelineBindPoint::eGraphics;

    return createInfo;
}

FPipelineCreateInfo PipelineConfig::CreatePrefiltredPipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};
    createInfo.vertexInputDesc = Vertex::getBindingDescription();
    createInfo.vertexAtribDesc = Vertex::getAttributeDescriptions();

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 1;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.vertexAtribDesc.size());

    createInfo.inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    createInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

    createInfo.viewport.x = 0.0f;
    createInfo.viewport.y = 0.0f;
    createInfo.viewport.width = (float)WindowHandle::m_iWidth;
    createInfo.viewport.height = (float)WindowHandle::m_iHeight;
    createInfo.viewport.minDepth = 0.0f;
    createInfo.viewport.maxDepth = 1.0f;

    createInfo.scissor.offset = vk::Offset2D{0, 0};
    createInfo.scissor.extent = vk::Extent2D{static_cast<uint32_t>(WindowHandle::m_iWidth), static_cast<uint32_t>(WindowHandle::m_iHeight)};

    createInfo.rasterizer.depthClampEnable = VK_FALSE;
    createInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
    createInfo.rasterizer.polygonMode = vk::PolygonMode::eFill;
    createInfo.rasterizer.lineWidth = 1.0f;
    createInfo.rasterizer.cullMode = vk::CullModeFlagBits::eNone;
    createInfo.rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
    createInfo.rasterizer.depthBiasEnable = VK_FALSE;

    createInfo.multisampling.rasterizationSamples = samples;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

    createInfo.colorBlendAttachments.emplace_back(colorBlendAttachment);

    createInfo.colorBlending.logicOpEnable = VK_FALSE;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());
    createInfo.colorBlending.blendConstants[0] = 0.0f;
    createInfo.colorBlending.blendConstants[1] = 0.0f;
    createInfo.colorBlending.blendConstants[2] = 0.0f;
    createInfo.colorBlending.blendConstants[3] = 0.0f;

    createInfo.depthStencil.depthTestEnable = VK_FALSE;
    createInfo.depthStencil.depthWriteEnable = VK_FALSE;
    createInfo.depthStencil.depthCompareOp = vk::CompareOp::eLessOrEqual;
    createInfo.depthStencil.back.compareOp = vk::CompareOp::eAlways;
    createInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
    createInfo.depthStencil.stencilTestEnable = VK_FALSE;

    createInfo.dynamicStateEnables = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    createInfo.dynamicStateInfo.pDynamicStates = createInfo.dynamicStateEnables.data();
    createInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(createInfo.dynamicStateEnables.size());
    createInfo.dynamicStateInfo.flags = vk::PipelineDynamicStateCreateFlags{};

    createInfo.pipelineLayout = pipelineLayout;
    createInfo.pipelineCache = pipelineCache;
    createInfo.renderPass = renderPass;

    createInfo.eType = EPipelineType::eGraphics;
    createInfo.eSet = EShaderSet::ePrefiltred;

    createInfo.bindPoint = vk::PipelineBindPoint::eGraphics;

    return createInfo;
}

FPipelineCreateInfo PipelineConfig::CreateDeferredPipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};
    //createInfo.vertexInputDesc = Vertex::getBindingDescription();
    //createInfo.vertexAtribDesc = Vertex::getAttributeDescriptions();

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 1;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.vertexAtribDesc.size());

    createInfo.inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    createInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

    createInfo.viewport.x = 0.0f;
    createInfo.viewport.y = 0.0f;
    createInfo.viewport.width = (float)WindowHandle::m_iWidth;
    createInfo.viewport.height = (float)WindowHandle::m_iHeight;
    createInfo.viewport.minDepth = 0.0f;
    createInfo.viewport.maxDepth = 1.0f;

    createInfo.scissor.offset = vk::Offset2D{0, 0};
    createInfo.scissor.extent = vk::Extent2D{static_cast<uint32_t>(WindowHandle::m_iWidth), static_cast<uint32_t>(WindowHandle::m_iHeight)};

    createInfo.rasterizer.depthClampEnable = VK_FALSE;
    createInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
    createInfo.rasterizer.polygonMode = vk::PolygonMode::eFill;
    createInfo.rasterizer.lineWidth = 1.0f;
    createInfo.rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    createInfo.rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
    createInfo.rasterizer.depthBiasEnable = VK_FALSE;

    createInfo.multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

    createInfo.colorBlendAttachments.emplace_back(colorBlendAttachment);

    createInfo.colorBlending.logicOpEnable = VK_FALSE;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());

    createInfo.depthStencil.depthTestEnable = VK_TRUE;
    createInfo.depthStencil.depthWriteEnable = VK_TRUE;
    createInfo.depthStencil.depthCompareOp = vk::CompareOp::eLessOrEqual;
    createInfo.depthStencil.back.compareOp = vk::CompareOp::eAlways;
    createInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
    createInfo.depthStencil.stencilTestEnable = VK_FALSE;

    createInfo.dynamicStateEnables = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    createInfo.dynamicStateInfo.pDynamicStates = createInfo.dynamicStateEnables.data();
    createInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(createInfo.dynamicStateEnables.size());
    createInfo.dynamicStateInfo.flags = vk::PipelineDynamicStateCreateFlags{};

    createInfo.pipelineLayout = pipelineLayout;
    createInfo.pipelineCache = pipelineCache;
    createInfo.renderPass = renderPass;

    createInfo.eType = EPipelineType::eGraphics;
    createInfo.eSet = EShaderSet::eDeferred;

    createInfo.bindPoint = vk::PipelineBindPoint::eGraphics;

    return createInfo;
}