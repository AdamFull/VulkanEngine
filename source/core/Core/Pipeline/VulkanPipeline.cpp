#include "VulkanPipeline.h"
#include "Core/VulkanHighLevel.h"
#include "filesystem/FilesystemHelper.h"

using namespace Engine::Core::Pipeline;

PipelineBase::~PipelineBase()
{
    Cleanup();
}

void PipelineBase::Create(FPipelineCreateInfo createInfo)
{
    savedInfo = std::move(createInfo);
    CreateDescriptorPool();
    CreateDescriptorSetLayout();
    CreatePipelineLayout();
}

void PipelineBase::RecreatePipeline(FPipelineCreateInfo createInfo)
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