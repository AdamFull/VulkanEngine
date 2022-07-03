#include "Pipeline.h"
#include "graphics/VulkanHighLevel.h"
#include "filesystem/FilesystemHelper.h"

using namespace engine::core::pipeline;

CPipelineBase::CPipelineBase()
{
    m_pShader = utl::make_scope<CShader>();
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
    loadShaders();
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

void CPipelineBase::setBindPoint(vk::PipelineBindPoint bindPoint)
{
    m_bindPoint = bindPoint;
}

void CPipelineBase::setVertexInput(CVertexInput &&vertexInput)
{
	m_vertexInput = std::move(vertexInput);
}

void CPipelineBase::setColorAttachments(uint32_t attachments)
{
	m_colorAttachments = attachments;
}

void CPipelineBase::setCulling(vk::CullModeFlagBits culling)
{
	m_culling = culling;
}

void CPipelineBase::setFontFace(vk::FrontFace fontface)
{
	m_fontface = fontface;
}

void CPipelineBase::setDepthEnabled(vk::Bool32 enableDepth)
{
	m_enableDepth = enableDepth;
}

void CPipelineBase::addDynamicState(vk::DynamicState state)
{
	m_vDynamicStateEnables.emplace_back(state);
}

void CPipelineBase::setDynamicStates(const std::vector<vk::DynamicState> &states)
{
	m_vDynamicStateEnables = states;
}

void CPipelineBase::addShaderStage(const std::string &stage)
{
	m_vStages.emplace_back(stage);
}

void CPipelineBase::setShaderStages(const std::vector<std::string> &stages)
{
	m_vStages = stages;
}

void CPipelineBase::addDefine(const std::string &name, const std::string &value)
{
	m_vDefines.emplace(std::make_pair(name, value));
}

void CPipelineBase::setDefines(const std::map<std::string, std::string> &defines)
{
    m_vDefines = defines;
}

void CPipelineBase::setTesselationEnabled(bool value)
{
    bEnableTesselation = value;
}

void CPipelineBase::addSpecializationInfo(vk::SpecializationInfo&& specInfo)
{
    vSpecInfos.emplace_back(std::move(specInfo));
}

void CPipelineBase::bind(vk::CommandBuffer &commandBuffer)
{
    commandBuffer.bindPipeline(getBindPoint(), getPipeline());
}

void CPipelineBase::loadShaders()
{
    std::stringstream defineBlock;
    for (const auto &[defineName, defineValue] : m_vDefines)
        defineBlock << "#define " << defineName << " " << defineValue << '\n';

    for (auto &stage : m_vStages)
    {
        auto shader_code = FilesystemHelper::readFile(stage);
        m_pShader->addStage(stage, shader_code, defineBlock.str());
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
    m_pShader->clear();
    loadShaders();
}