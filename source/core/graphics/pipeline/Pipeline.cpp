#include "Pipeline.h"
#include "graphics/VulkanHighLevel.h"
#include "filesystem/FilesystemHelper.h"

using namespace Engine::Core::Pipeline;

std::unique_ptr<PipelineBase> PipelineBase::Builder::build(vk::RenderPass& renderPass, uint32_t subpass)
{
    std::unique_ptr<PipelineBase> pPipeline = std::make_unique<GraphicsPipeline>();

    pPipeline->m_vertexInput = std::move(m_vertexInput);
    pPipeline->m_renderPass = renderPass;
    pPipeline->subpass = subpass;
    pPipeline->m_bindPoint = vk::PipelineBindPoint::eGraphics;
    pPipeline->m_colorAttachments = m_colorAttachments;
    pPipeline->m_culling = m_culling;
    pPipeline->m_fontface = m_fontface;
    pPipeline->m_enableDepth = m_enableDepth;
    pPipeline->m_vDynamicStateEnables = m_vDynamicStateEnables;
    pPipeline->m_vDefines = std::move(m_vDefines);
    pPipeline->m_vStages = std::move(m_vStages);
    pPipeline->Create();
    return pPipeline;
}

std::unique_ptr<PipelineBase> PipelineBase::Builder::build()
{
    std::unique_ptr<PipelineBase> pPipeline = std::make_unique<ComputePipeline>();
    pPipeline->m_bindPoint = vk::PipelineBindPoint::eCompute;
    pPipeline->m_vDefines = std::move(m_vDefines);
    pPipeline->m_vStages = std::move(m_vStages);
    pPipeline->Create();
    return pPipeline;
}


PipelineBase::~PipelineBase()
{
    Cleanup();
}

void PipelineBase::Create()
{
    LoadShader(m_vStages);
    CreateDescriptorPool();
    CreateDescriptorSetLayout();
    CreatePipelineLayout();
}

void PipelineBase::RecreatePipeline()
{
}

void PipelineBase::Cleanup()
{
    UDevice->Destroy(m_pipeline);
    UDevice->Destroy(m_pipelineLayout);
}

void PipelineBase::Bind(vk::CommandBuffer &commandBuffer)
{
    commandBuffer.bindPipeline(GetBindPoint(), GetPipeline());
}

void PipelineBase::LoadShader(const std::vector<std::string> &vShaders)
{
    m_pShader = std::make_unique<Shader>();
    std::stringstream defineBlock;
    for (const auto &[defineName, defineValue] : m_vDefines)
        defineBlock << "#define " << defineName << " " << defineValue << '\n';

    for (auto &value : vShaders)
    {
        m_vShaderCache.emplace_back(value);
        auto shader_code = FilesystemHelper::ReadFile(value);
        m_pShader->AddStage(value, shader_code, defineBlock.str());
    }
    m_pShader->BuildReflection();
}

void PipelineBase::CreateDescriptorSetLayout()
{
    auto &descriptorSetLayouts = m_pShader->GetDescriptorSetLayouts();

	vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayouts.data();

    if (UDevice->GetLogical().createDescriptorSetLayout(&descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout) != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void PipelineBase::CreateDescriptorPool()
{
    auto &descriptorPools = m_pShader->GetDescriptorPools();

	vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	descriptorPoolCreateInfo.maxSets = 8192; // 16384;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPools.size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPools.data();
    m_descriptorPool = UDevice->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(descriptorPoolCreateInfo);
}

void PipelineBase::CreatePipelineLayout()
{
    auto pushConstantRanges = m_pShader->GetPushConstantRanges();

	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &m_descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
	pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
    m_pipelineLayout = UDevice->Make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutCreateInfo);
}

void PipelineBase::RecreateShaders()
{
    std::vector<std::string> vCacheCopy = m_vShaderCache;
    m_vShaderCache.clear();
    m_pShader->Clear();
    LoadShader(vCacheCopy);
}