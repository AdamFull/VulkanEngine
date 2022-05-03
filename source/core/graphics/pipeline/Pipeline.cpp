#include "Pipeline.h"
#include "graphics/VulkanHighLevel.h"
#include "filesystem/FilesystemHelper.h"

using namespace Engine::Core::Pipeline;

std::unique_ptr<CPipelineBase> CPipelineBase::Builder::build(vk::PipelineBindPoint bindPoint)
{
    std::unique_ptr<CPipelineBase> pPipeline;
    
    switch (bindPoint)
    {
    case vk::PipelineBindPoint::eCompute: pPipeline = std::make_unique<CComputePipeline>(); break;
    case vk::PipelineBindPoint::eGraphics: pPipeline = std::make_unique<CGraphicsPipeline>(); break;
    case vk::PipelineBindPoint::eRayTracingKHR: pPipeline = std::make_unique<CComputePipeline>(); break;
    case vk::PipelineBindPoint::eSubpassShadingHUAWEI: pPipeline = std::make_unique<CComputePipeline>(); break;
    }

    pPipeline->m_bindPoint = bindPoint;
    pPipeline->m_vDefines = std::move(m_vDefines);
    pPipeline->m_vStages = std::move(m_vStages);

    if(bindPoint == vk::PipelineBindPoint::eGraphics)
    {
        pPipeline->m_vertexInput = std::move(m_vertexInput);
        pPipeline->m_colorAttachments = m_colorAttachments;
        pPipeline->m_culling = m_culling;
        pPipeline->m_fontface = m_fontface;
        pPipeline->m_enableDepth = m_enableDepth;
        pPipeline->m_vDynamicStateEnables = m_vDynamicStateEnables;
    }    
    
    return pPipeline;
}

CPipelineBase::~CPipelineBase()
{
    cleanup();
}

void CPipelineBase::create(vk::RenderPass& renderPass, uint32_t _subpass)
{
    m_renderPass = renderPass;
    subpass = _subpass;
    create();
}

void CPipelineBase::create()
{
    loadShader(m_vStages);
    createDescriptorPool();
    createDescriptorSetLayout();
    createPipelineLayout();
}

void CPipelineBase::recreatePipeline()
{
}

void CPipelineBase::cleanup()
{
    UDevice->destroy(m_pipeline);
    UDevice->destroy(m_pipelineLayout);
}

void CPipelineBase::bind(vk::CommandBuffer &commandBuffer)
{
    commandBuffer.bindPipeline(getBindPoint(), getPipeline());
}

void CPipelineBase::loadShader(const std::vector<std::string> &vShaders)
{
    m_pShader = std::make_unique<CShader>();
    std::stringstream defineBlock;
    for (const auto &[defineName, defineValue] : m_vDefines)
        defineBlock << "#define " << defineName << " " << defineValue << '\n';

    for (auto &value : vShaders)
    {
        m_vShaderCache.emplace_back(value);
        auto shader_code = FilesystemHelper::readFile(value);
        m_pShader->addStage(value, shader_code, defineBlock.str());
    }
    m_pShader->finalizeReflection();
}

void CPipelineBase::createDescriptorSetLayout()
{
    auto &descriptorSetLayouts = m_pShader->getDescriptorSetLayouts();

	vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayouts.data();

    if (UDevice->getLogical().createDescriptorSetLayout(&descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout) != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void CPipelineBase::createDescriptorPool()
{
    auto &descriptorPools = m_pShader->getDescriptorPools();

	vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	descriptorPoolCreateInfo.maxSets = 8192; // 16384;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPools.size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPools.data();
    m_descriptorPool = UDevice->make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(descriptorPoolCreateInfo);
}

void CPipelineBase::createPipelineLayout()
{
    auto pushConstantRanges = m_pShader->getPushConstantRanges();

	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &m_descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
	pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
    m_pipelineLayout = UDevice->make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutCreateInfo);
}

void CPipelineBase::recreateShaders()
{
    std::vector<std::string> vCacheCopy = m_vShaderCache;
    m_vShaderCache.clear();
    m_pShader->clear();
    loadShader(vCacheCopy);
}