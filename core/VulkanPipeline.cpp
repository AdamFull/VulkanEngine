#include "VulkanPipeline.h"
#include "filesystem/FilesystemHelper.h"

namespace Engine
{
    void VulkanPipeline::LoadShader(const std::string& srShaderPath, vk::ShaderStageFlagBits fShaderType)
    {
        auto shader_code = FilesystemHelper::ReadFile(srShaderPath);

        auto cached = FShaderCache{ fShaderType, shader_code};
        auto it = std::find_if(std::begin(m_vShaderCache), std::end(m_vShaderCache), 
        [&cached](FShaderCache &rhs)
        {
            return rhs == cached;
        });
        if(it == std::end(m_vShaderCache))
            m_vShaderCache.emplace_back(cached);

        LoadShader(shader_code, fShaderType);
    }

    void VulkanPipeline::LoadShader(const shader_load_map_t& mShaders)
    {
        for(auto& [key, value]: mShaders)
        {
            LoadShader(value, key);
        }
    }

    void VulkanPipeline::LoadShader(const std::vector<char>& vShaderCode, vk::ShaderStageFlagBits fShaderType) 
    {
        vk::ShaderModule shaderModule;

        try
        {
            /*shaderModule = m_pDevice->createShaderModule
            (
                {
                    vk::ShaderModuleCreateFlags(),
                    vShaderCode.size(),
                    reinterpret_cast<const uint32_t *>(vShaderCode.data())
                }
            );*/
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
            srShaderEntriePoint
        );
    }

    void VulkanPipeline::RecreateShaders()
    {
        m_vShaderBuffer.clear();

        for(auto& cached : m_vShaderCache)
        {
            LoadShader(cached.srShaderData, cached.sShaderType);
        }
    }
}