#pragma once
#include "VulkanMainStructures.h"

namespace Engine::Pipeline
{
    typedef std::map<vk::ShaderStageFlagBits, std::string> SaderMap;
    
    FPipelineConfigInfo PipelineDefault(uint32_t width, uint32_t height);

    void LoadShader(Main::FVulkanEngine& engine, const std::string& srShaderPath, vk::ShaderStageFlagBits fShaderType);
    void LoadShader(Main::FVulkanEngine& engine, const SaderMap& mShaders);

    void RecreateShaders(Main::FVulkanEngine& engine);
    void LoadShader(Main::FVulkanEngine& engine, const std::vector<char>& vShaderCode, vk::ShaderStageFlagBits fShaderType);
}