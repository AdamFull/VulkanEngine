#include "PipelineConfig.h"
#include "Renderer/WindowHandle.h"
#include "Renderer/DataTypes/VulkanVertex.hpp"

namespace Engine
{
    FPipelineCreateInfo PipelineConfig::CreatePipelineConfig(EPipelineType eType, EShaderSet eSet,
                                                             vk::PrimitiveTopology topology, vk::PolygonMode polygonMode, 
                                                             vk::CullModeFlags cullMode, vk::FrontFace fontFace, 
                                                             vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout,
                                                             vk::PipelineCache pipelineCache)
    {
        FPipelineCreateInfo createInfo{};

        switch(eSet)
        {
            case EShaderSet::eUI:
            {
                createInfo.vertexInputDesc = VertexUI::getBindingDescription();
                createInfo.vertexAtribDesc = VertexUI::getAttributeDescriptions();
            }break;
            case EShaderSet::eDiffuse:
            {
                createInfo.vertexInputDesc = Vertex::getBindingDescription();
                createInfo.vertexAtribDesc = Vertex::getAttributeDescriptions();
            }break;
        }

        createInfo.inputAssembly.topology = topology;
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
        createInfo.rasterizer.polygonMode = polygonMode;
        createInfo.rasterizer.lineWidth = 1.0f;
        createInfo.rasterizer.cullMode = cullMode;
        createInfo.rasterizer.frontFace = fontFace;
        createInfo.rasterizer.depthBiasEnable = VK_FALSE;

        createInfo.multisampling.sampleShadingEnable = VK_TRUE;
        createInfo.multisampling.minSampleShading = .2f;
        createInfo.multisampling.rasterizationSamples = samples;

        createInfo.colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        createInfo.colorBlendAttachment.blendEnable = VK_TRUE;
        createInfo.colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        createInfo.colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        createInfo.colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
        createInfo.colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        createInfo.colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        createInfo.colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

        createInfo.colorBlending.logicOpEnable = VK_FALSE;
        createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
        createInfo.colorBlending.attachmentCount = 1;
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

        createInfo.eType = eType;
        createInfo.eSet = eSet;

        return createInfo;
    }

    FPipelineCreateInfo PipelineConfig::CreateDefaultPipelineConfig(EPipelineType eType, EShaderSet eSet,vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
    {
        FPipelineCreateInfo createInfo = CreatePipelineConfig(eType, eSet, vk::PrimitiveTopology::eTriangleList, vk::PolygonMode::eFill,
                                    vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise, samples, pipelineLayout, pipelineCache);
        createInfo.bindPoint = vk::PipelineBindPoint::eGraphics;
        return createInfo;
    }

    FPipelineCreateInfo PipelineConfig::CreateDefaultDebugPipelineConfig(EPipelineType eType, EShaderSet eSet,vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout, vk::PipelineCache pipelineCache)
    {
        FPipelineCreateInfo createInfo = CreatePipelineConfig(eType, eSet, vk::PrimitiveTopology::eTriangleList, vk::PolygonMode::eLine,
                                    vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, samples, pipelineLayout, pipelineCache);
        createInfo.bindPoint = vk::PipelineBindPoint::eGraphics;
        return createInfo;
    }
}