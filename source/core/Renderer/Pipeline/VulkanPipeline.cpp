#include "VulkanPipeline.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanSwapChain.h"
#include "filesystem/FilesystemHelper.h"

namespace Engine
{
    void PipelineBase::Create(FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        
    }

    void PipelineBase::RecreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        
    }

    void PipelineBase::Cleanup(std::unique_ptr<Device>& device)
    {
        device->Destroy(data.pipeline);
        device->Destroy(data.layout);
        device->GetLogical()->freeDescriptorSets(data.descriptorPool, data.vDescriptorSets);
        device->Destroy(data.descriptorSetLayout);
        device->Destroy(data.descriptorPool);
    }

    void PipelineBase::Destroy(std::unique_ptr<Device>& device)
    {
        Cleanup(device);
    }

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