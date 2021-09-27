#include "VulkanPipeline.h"
#include "core/VulkanDevice.h"
#include "core/VulkanSwapChain.h"
#include "filesystem/FilesystemHelper.h"

namespace Engine
{
    void PipelineBase::Bind(vk::CommandBuffer& commandBuffer)
    {
        commandBuffer.bindPipeline(GetBindPoint(), GetPipeline());
    }

    void PipelineBase::LoadShader(std::unique_ptr<Device>& device, const std::string& srShaderPath, vk::ShaderStageFlagBits fShaderType)
    {
        auto shader_code = FilesystemHelper::ReadFile(srShaderPath);
        m_vShaderCache.emplace_back(FShaderCache{ fShaderType, shader_code});
        LoadShader(device, shader_code, fShaderType);
    }

    void PipelineBase::LoadShader(std::unique_ptr<Device>& device, const std::map<vk::ShaderStageFlagBits, std::string>& mShaders)
    {
        for(auto& [key, value]: mShaders)
        {
            LoadShader(device, value, key);
        }
    }

    void PipelineBase::LoadShader(std::unique_ptr<Device>& device, const std::vector<char>& vShaderCode, vk::ShaderStageFlagBits fShaderType) 
    {
        vk::ShaderModule shaderModule;

        try
        {
            shaderModule = device->Make<vk::ShaderModule, vk::ShaderModuleCreateInfo>
            (
                vk::ShaderModuleCreateInfo
                {
                    vk::ShaderModuleCreateFlags(),
                    vShaderCode.size(),
                    reinterpret_cast<const uint32_t *>(vShaderCode.data())
                }
            );
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create shader module!");
        }

        m_vShaderBuffer.emplace_back
        (
            vk::PipelineShaderStageCreateFlags(),
            fShaderType,
            shaderModule,
            "main"
        );
    }

    void PipelineBase::RecreateShaders(std::unique_ptr<Device>& device)
    {
        m_vShaderBuffer.clear();

        for(auto& cached : m_vShaderCache)
        {
            LoadShader(device, cached.srShaderData, cached.sShaderType);
        }
    }
}