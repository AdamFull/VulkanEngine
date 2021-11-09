#include "VulkanPipeline.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanSwapChain.h"
#include "filesystem/FilesystemHelper.h"

using namespace Engine::Core::Pipeline;

PipelineBase::PipelineBase(std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapchain) :
m_device(device),
m_swapchain(swapchain)
{

}

PipelineBase::~PipelineBase()
{
    Cleanup();
    DestroyShaders();
}

void PipelineBase::Create(FPipelineCreateInfo createInfo)
{
}

void PipelineBase::RecreatePipeline(FPipelineCreateInfo createInfo)
{
}

void PipelineBase::Cleanup()
{
    m_device->Destroy(data.pipeline);
}

void PipelineBase::Bind(vk::CommandBuffer &commandBuffer)
{
    commandBuffer.bindPipeline(GetBindPoint(), GetPipeline());
}

void PipelineBase::LoadShader(const std::string &srShaderPath, vk::ShaderStageFlagBits fShaderType)
{
    auto shader_code = FilesystemHelper::ReadFile(srShaderPath);
    m_vShaderCache.emplace_back(FShaderCache{fShaderType, shader_code});
    LoadShader(shader_code, fShaderType);
}

void PipelineBase::LoadShader(const std::map<vk::ShaderStageFlagBits, std::string> &mShaders)
{
    for (auto &[key, value] : mShaders)
    {
        LoadShader(value, key);
    }
}

void PipelineBase::LoadShader(const std::vector<char> &vShaderCode, vk::ShaderStageFlagBits fShaderType)
{
    vk::ShaderModule shaderModule;

    try
    {
        shaderModule = m_device->Make<vk::ShaderModule, vk::ShaderModuleCreateInfo>(
            vk::ShaderModuleCreateInfo{
                vk::ShaderModuleCreateFlags(),
                vShaderCode.size(),
                reinterpret_cast<const uint32_t *>(vShaderCode.data())});
    }
    catch (vk::SystemError err)
    {
        throw std::runtime_error("Failed to create shader module!");
    }

    m_vShaderBuffer.emplace_back(
        vk::PipelineShaderStageCreateFlags(),
        fShaderType,
        shaderModule,
        "main");
}

void PipelineBase::RecreateShaders()
{
    DestroyShaders();
    for (auto &cached : m_vShaderCache)
    {
        LoadShader(cached.srShaderData, cached.sShaderType);
    }
}

void PipelineBase::DestroyShaders()
{
    for (auto &stageInfo : m_vShaderBuffer)
    {
        m_device->Destroy(stageInfo.module);
    }

    m_vShaderBuffer.clear();
}