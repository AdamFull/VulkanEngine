#include "PipelineConfig.h"
#include "Core/Window/WindowHandle.h"
#include "Core/VulkanInitializers.h"
#include "Core/DataTypes/VulkanVertex.hpp"

using namespace Engine::Core::Pipeline;
using namespace Engine::Core::Window;

FPipelineCreateInfo PipelineConfig::CreateUIPipeline(vk::RenderPass renderPass, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};
    createInfo.vertexInputDesc = VertexUI::getBindingDescription();
    createInfo.vertexAtribDesc = VertexUI::getAttributeDescriptions();

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 1;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.vertexAtribDesc.size());

    createInfo.inputAssembly = Initializers::PipelineInputAssemblyStateCI(vk::PrimitiveTopology::eTriangleList, VK_FALSE);

    createInfo.viewport = Initializers::Viewport(WindowHandle::m_iWidth, WindowHandle::m_iHeight);
    createInfo.scissor = Initializers::Scissor(WindowHandle::m_iWidth, WindowHandle::m_iHeight, 0, 0);

    createInfo.rasterizer = Initializers::PipelineRasterizerStateCI(vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise);

    createInfo.multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment = 
    Initializers::PipelineColorBlendAttachmentState(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA, VK_TRUE);
    createInfo.colorBlendAttachments.push_back(colorBlendAttachment);
    
    createInfo.colorBlending.logicOpEnable = VK_FALSE;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());

    createInfo.depthStencil = Initializers::PipelineDepthStencilStateCI(VK_TRUE, VK_TRUE, vk::CompareOp::eLessOrEqual);

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

FPipelineCreateInfo PipelineConfig::CreateDiffusePipeline(vk::RenderPass renderPass, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};
    createInfo.vertexInputDesc = Vertex::getBindingDescription();
    createInfo.vertexAtribDesc = Vertex::getAttributeDescriptions();

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 1;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.vertexAtribDesc.size());

    createInfo.inputAssembly = Initializers::PipelineInputAssemblyStateCI(vk::PrimitiveTopology::eTriangleList, VK_FALSE);

    createInfo.viewport = Initializers::Viewport(WindowHandle::m_iWidth, WindowHandle::m_iHeight);
    createInfo.scissor = Initializers::Scissor(WindowHandle::m_iWidth, WindowHandle::m_iHeight, 0, 0);

    createInfo.rasterizer = Initializers::PipelineRasterizerStateCI(vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise);

    createInfo.multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    for(uint32_t i = 0; i < 3; i++)
    {
        vk::PipelineColorBlendAttachmentState colorBlendAttachment =
        Initializers::PipelineColorBlendAttachmentState(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA, VK_TRUE);
        createInfo.colorBlendAttachments.emplace_back(colorBlendAttachment);
    }

    createInfo.colorBlending.logicOpEnable = VK_FALSE;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());

    createInfo.depthStencil = Initializers::PipelineDepthStencilStateCI(VK_TRUE, VK_TRUE, vk::CompareOp::eLessOrEqual);

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

FPipelineCreateInfo PipelineConfig::CreateSkyboxPipeline(vk::RenderPass renderPass, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};
    createInfo.vertexInputDesc = Vertex::getBindingDescription();
    createInfo.vertexAtribDesc = Vertex::getAttributeDescriptions();

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 1;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.vertexAtribDesc.size());

    createInfo.inputAssembly = Initializers::PipelineInputAssemblyStateCI(vk::PrimitiveTopology::eTriangleList, VK_FALSE);

    createInfo.rasterizer = Initializers::PipelineRasterizerStateCI(vk::PolygonMode::eFill, vk::CullModeFlagBits::eFront, vk::FrontFace::eCounterClockwise);

    createInfo.multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    for(uint32_t i = 0; i < 3; i++)
    {
        vk::PipelineColorBlendAttachmentState colorBlendAttachment = 
        Initializers::PipelineColorBlendAttachmentState(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA, VK_FALSE);
        createInfo.colorBlendAttachments.emplace_back(colorBlendAttachment);
    }

    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());

    createInfo.depthStencil = Initializers::PipelineDepthStencilStateCI(VK_FALSE, VK_FALSE, vk::CompareOp::eLessOrEqual);

    createInfo.viewport = Initializers::Viewport(WindowHandle::m_iWidth, WindowHandle::m_iHeight);
    createInfo.scissor = Initializers::Scissor(WindowHandle::m_iWidth, WindowHandle::m_iHeight, 0, 0);

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

FPipelineCreateInfo PipelineConfig::CreateBRDFPipeline(vk::RenderPass renderPass, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};
    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.inputAssembly = Initializers::PipelineInputAssemblyStateCI(vk::PrimitiveTopology::eTriangleList, VK_FALSE);

    createInfo.viewport = Initializers::Viewport(WindowHandle::m_iWidth, WindowHandle::m_iHeight);
    createInfo.scissor = Initializers::Scissor(WindowHandle::m_iWidth, WindowHandle::m_iHeight, 0, 0);

    createInfo.rasterizer = Initializers::PipelineRasterizerStateCI(vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise);

    createInfo.multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment =
    Initializers::PipelineColorBlendAttachmentState(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA, VK_TRUE);

    createInfo.colorBlendAttachments.emplace_back(colorBlendAttachment);

    createInfo.colorBlending.logicOpEnable = VK_FALSE;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());

    createInfo.depthStencil = Initializers::PipelineDepthStencilStateCI(VK_FALSE, VK_FALSE, vk::CompareOp::eLessOrEqual);

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

FPipelineCreateInfo PipelineConfig::CreateIrradiatePipeline(vk::RenderPass renderPass, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};
    createInfo.vertexInputDesc = Vertex::getBindingDescription();
    createInfo.vertexAtribDesc = Vertex::getAttributeDescriptions();

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 1;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.vertexAtribDesc.size());

    createInfo.inputAssembly = Initializers::PipelineInputAssemblyStateCI(vk::PrimitiveTopology::eTriangleList, VK_FALSE);

    createInfo.viewport = Initializers::Viewport(WindowHandle::m_iWidth, WindowHandle::m_iHeight);
    createInfo.scissor = Initializers::Scissor(WindowHandle::m_iWidth, WindowHandle::m_iHeight, 0, 0);

    createInfo.rasterizer = Initializers::PipelineRasterizerStateCI(vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise);

    createInfo.multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment =
    Initializers::PipelineColorBlendAttachmentState(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA, VK_TRUE);

    createInfo.colorBlendAttachments.emplace_back(colorBlendAttachment);

    createInfo.colorBlending.logicOpEnable = VK_FALSE;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());

    createInfo.depthStencil = Initializers::PipelineDepthStencilStateCI(VK_FALSE, VK_FALSE, vk::CompareOp::eLessOrEqual);

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

FPipelineCreateInfo PipelineConfig::CreatePrefiltredPipeline(vk::RenderPass renderPass, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};
    createInfo.vertexInputDesc = Vertex::getBindingDescription();
    createInfo.vertexAtribDesc = Vertex::getAttributeDescriptions();

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 1;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.vertexAtribDesc.size());

    createInfo.inputAssembly = Initializers::PipelineInputAssemblyStateCI(vk::PrimitiveTopology::eTriangleList, VK_FALSE);

    createInfo.viewport = Initializers::Viewport(WindowHandle::m_iWidth, WindowHandle::m_iHeight);
    createInfo.scissor = Initializers::Scissor(WindowHandle::m_iWidth, WindowHandle::m_iHeight, 0, 0);

    createInfo.rasterizer = Initializers::PipelineRasterizerStateCI(vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise);

    createInfo.multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment =
    Initializers::PipelineColorBlendAttachmentState(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA, VK_TRUE);

    createInfo.colorBlendAttachments.emplace_back(colorBlendAttachment);

    createInfo.colorBlending.logicOpEnable = VK_FALSE;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());

    createInfo.depthStencil = Initializers::PipelineDepthStencilStateCI(VK_FALSE, VK_FALSE, vk::CompareOp::eLessOrEqual);

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

FPipelineCreateInfo PipelineConfig::CreateDeferredPipeline(vk::RenderPass renderPass, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
{
    FPipelineCreateInfo createInfo{};

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    createInfo.vertexInputInfo.vertexBindingDescriptionCount = 1;
    createInfo.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.vertexAtribDesc.size());

    createInfo.inputAssembly = Initializers::PipelineInputAssemblyStateCI(vk::PrimitiveTopology::eTriangleList, VK_FALSE);
    createInfo.viewport = Initializers::Viewport(WindowHandle::m_iWidth, WindowHandle::m_iHeight);
    createInfo.scissor = Initializers::Scissor(WindowHandle::m_iWidth, WindowHandle::m_iHeight, 0, 0);

    createInfo.rasterizer = Initializers::PipelineRasterizerStateCI(vk::PolygonMode::eFill, vk::CullModeFlagBits::eFront, vk::FrontFace::eCounterClockwise);

    createInfo.multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment = Initializers::PipelineColorBlendAttachmentState(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA, VK_FALSE);
    createInfo.colorBlendAttachments.emplace_back(colorBlendAttachment);

    createInfo.colorBlending.logicOpEnable = VK_FALSE;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorBlendAttachments.size());

    createInfo.depthStencil = Initializers::PipelineDepthStencilStateCI(VK_TRUE, VK_TRUE, vk::CompareOp::eLessOrEqual);

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