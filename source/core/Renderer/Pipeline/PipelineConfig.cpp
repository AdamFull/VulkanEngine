#include "PipelineConfig.h"
#include "WindowHandle.h"

namespace Engine
{
    FPipelineCreateInfo PipelineConfig::CreatePipelineConfig(vk::PrimitiveTopology topology, vk::PolygonMode polygonMode, 
                                                             vk::CullModeFlags cullMode, vk::FrontFace fontFace, 
                                                             vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout)
    {
        FPipelineCreateInfo createInfo{};

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

        createInfo.pipelineLayout = pipelineLayout;

        return createInfo;
    }

    FPipelineCreateInfo PipelineConfig::CreateDefaultPipelineConfig(vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout)
    {
        FPipelineCreateInfo createInfo = CreatePipelineConfig(vk::PrimitiveTopology::eTriangleList, vk::PolygonMode::eFill,
                                    vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, samples, pipelineLayout);
        createInfo.bindPoint = vk::PipelineBindPoint::eGraphics;
        return createInfo;
    }

    FPipelineCreateInfo PipelineConfig::CreateDefaultDebugPipelineConfig(vk::SampleCountFlagBits samples, vk::PipelineLayout pipelineLayout)
    {
        FPipelineCreateInfo createInfo = CreatePipelineConfig(vk::PrimitiveTopology::eTriangleList, vk::PolygonMode::eLine,
                                    vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, samples, pipelineLayout);
        createInfo.bindPoint = vk::PipelineBindPoint::eGraphics;
        return createInfo;
    }
}