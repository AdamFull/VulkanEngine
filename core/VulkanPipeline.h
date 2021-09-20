#pragma once

namespace Engine
{
    constexpr const char* srShaderEntriePoint = "main";

    struct FShaderCache
    {
        bool operator==(FShaderCache &rhs)
        {
            return rhs.sShaderType == this->sShaderType &&
                   rhs.srShaderData == this->srShaderData;
        }

        vk::ShaderStageFlagBits sShaderType;
        std::vector<char> srShaderData;
    };

    class VulkanPipeline
    {
    public:
        using shader_load_map_t = std::map<vk::ShaderStageFlagBits, std::string>;

        void LoadShader(const std::string& srShaderPath, vk::ShaderStageFlagBits fShaderType);
        void LoadShader(const shader_load_map_t& mShaders);
    private:
        void RecreateShaders();
        void LoadShader(const std::vector<char>& vShaderCode, vk::ShaderStageFlagBits fShaderType);
    private:
        std::vector<FShaderCache> m_vShaderCache;
        std::vector<vk::PipelineShaderStageCreateInfo> m_vShaderBuffer;
    };
}