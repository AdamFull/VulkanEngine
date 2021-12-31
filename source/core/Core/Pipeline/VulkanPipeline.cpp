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
}

void PipelineBase::RecreatePipeline(FPipelineCreateInfo createInfo)
{
}

void PipelineBase::Cleanup()
{
    UDevice->Destroy(m_pipeline);
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

void PipelineBase::RecreateShaders()
{
    std::vector<std::string> vCacheCopy = m_vShaderCache;
    m_vShaderCache.clear();
    m_pShader->Clear();
    LoadShader(vCacheCopy);
}