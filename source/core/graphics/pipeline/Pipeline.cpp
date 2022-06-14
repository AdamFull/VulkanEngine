#include "Pipeline.h"
#include "graphics/VulkanHighLevel.h"
#include "filesystem/FilesystemHelper.h"

using namespace engine::core::pipeline;

scope_ptr<CPipelineBase> CPipelineBase::Builder::build(vk::PipelineBindPoint bindPoint)
{
    scope_ptr<CPipelineBase> pPipeline;
    
    switch (bindPoint)
    {
    case vk::PipelineBindPoint::eCompute: pPipeline = make_scope<CComputePipeline>(); break;
    case vk::PipelineBindPoint::eGraphics: pPipeline = make_scope<CGraphicsPipeline>(); break;
    case vk::PipelineBindPoint::eRayTracingKHR: pPipeline = make_scope<CComputePipeline>(); break;
    case vk::PipelineBindPoint::eSubpassShadingHUAWEI: pPipeline = make_scope<CComputePipeline>(); break;
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

void CPipelineBase::reCreate(vk::RenderPass& renderPass, uint32_t _subpass)
{
    m_renderPass = renderPass;
    subpass = _subpass;
    recreateShaders();
    createPipeline();
}

void CPipelineBase::cleanup()
{
    if(!bIsClean)
    {
        if(descriptorSetLayout)
            CDevice::inst()->destroy(&descriptorSetLayout);
        if(descriptorPool)
            CDevice::inst()->destroy(&descriptorPool);
        if(pipeline)
            CDevice::inst()->destroy(&pipeline);
        if(pipelineLayout)
            CDevice::inst()->destroy(&pipelineLayout);
        bIsClean = true;
    }
}

void CPipelineBase::addDefine(const std::string& define, const std::string& value)
{
    m_vDefines.emplace(define, value);
}

void CPipelineBase::bind(vk::CommandBuffer &commandBuffer)
{
    commandBuffer.bindPipeline(getBindPoint(), getPipeline());
}

void CPipelineBase::loadShader(const std::vector<std::string> &vShaders)
{
    m_pShader = make_scope<CShader>();
    std::stringstream defineBlock;
    for (const auto &[defineName, defineValue] : m_vDefines)
        defineBlock << "#define " << defineName << " " << defineValue << '\n';

    for (auto &value : vShaders)
    {
        vShaderCache.emplace_back(value);
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

    vk::Result res = CDevice::inst()->create(descriptorSetLayoutCreateInfo, &descriptorSetLayout);
    assert(res == vk::Result::eSuccess && "Cannot create descriptor set layout.");
}

void CPipelineBase::createDescriptorPool()
{
    auto &descriptorPools = m_pShader->getDescriptorPools();

	vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	descriptorPoolCreateInfo.maxSets = 8192; // 16384;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPools.size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPools.data();
    vk::Result res = CDevice::inst()->create(descriptorPoolCreateInfo, &descriptorPool);
    assert(res == vk::Result::eSuccess && "Cannot create descriptor pool.");
}

void CPipelineBase::createPipelineLayout()
{
    auto pushConstantRanges = m_pShader->getPushConstantRanges();

	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
	pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
    vk::Result res = CDevice::inst()->create(pipelineLayoutCreateInfo, &pipelineLayout);
    assert(res == vk::Result::eSuccess && "Cannot create pipeline layout.");
}

void CPipelineBase::recreateShaders()
{
    std::vector<std::string> vCacheCopy = vShaderCache;
    vShaderCache.clear();
    m_pShader->clear();
    loadShader(vCacheCopy);
}