#pragma once
#include "VulkanMainStructures.h"

namespace Engine::Pipeline
{
    using shader_load_map_t = std::map<vk::ShaderStageFlagBits, std::string>;

    FPipelineConfigInfo PipelineDefault(uint32_t width, uint32_t height);

    void LoadShader(const std::string& srShaderPath, vk::ShaderStageFlagBits fShaderType);
    void LoadShader(const shader_load_map_t& mShaders);

    void RecreateShaders();
    void LoadShader(const std::vector<char>& vShaderCode, vk::ShaderStageFlagBits fShaderType);
}