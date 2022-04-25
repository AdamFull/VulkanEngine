#include "Pipeline.h"
#include "graphics/VulkanHighLevel.h"
#include "filesystem/FilesystemHelper.h"

using namespace Engine::Core::Pipeline;

std::unique_ptr<PipelineBase> PipelineBase::Builder::Build(vk::PipelineBindPoint bindPoint)
{
    std::unique_ptr<PipelineBase> pPipeline;
    switch (bindPoint)
    {
    case vk::PipelineBindPoint::eGraphics: pPipeline = std::make_unique<GraphicsPipeline>(); break;
    case vk::PipelineBindPoint::eCompute: pPipeline = std::make_unique<ComputePipeline>(); break;
    case vk::PipelineBindPoint::eRayTracingKHR: break;
    case vk::PipelineBindPoint::eSubpassShadingHUAWEI: break;
    //case vk::PipelineBindPoint::eRayTracingNV: break;
    }    

    switch (bindPoint)
    {
    case vk::PipelineBindPoint::eGraphics: {
        pPipeline->Create(std::move(m_vertexInput), m_renderPass, bindPoint, m_colorAttachments, m_culling, m_fontface, m_enableDepth, m_vDynamicStateEnables, m_vDefines, m_vStages);
    }break;
    case vk::PipelineBindPoint::eCompute: {
        pPipeline->Create(bindPoint, m_vDefines, m_vStages);
    }break;
    }
    return pPipeline;
}


PipelineBase::~PipelineBase()
{
    Cleanup();
}

void PipelineBase::Create(VertexInput&& vertexInput, vk::RenderPass& renderPass, vk::PipelineBindPoint bindPoint, uint32_t colorAttachments, vk::CullModeFlagBits culling, vk::FrontFace fontface, 
                          vk::Bool32 enableDepth, const std::vector<vk::DynamicState>& vDynamicStateEnables, const std::vector<Shader::Define>& vDefines, const std::vector<std::string>& vStages)
{
    m_vertexInput = vertexInput;
    m_renderPass = renderPass;
    m_bindPoint = bindPoint;
    m_colorAttachments = colorAttachments;
    m_culling = culling;
    m_fontface = fontface;
    m_enableDepth = enableDepth;
    m_vDynamicStateEnables = vDynamicStateEnables;
    m_vDefines = vDefines;

    LoadShader(vStages);
    CreateDescriptorPool();
    CreateDescriptorSetLayout();
    CreatePipelineLayout();
}

void PipelineBase::Create(vk::PipelineBindPoint bindPoint,  const std::vector<Shader::Define>& vDefines, const std::vector<std::string>& vStages)
{
    m_bindPoint = bindPoint;
    m_vDefines = vDefines;

    LoadShader(vStages);
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