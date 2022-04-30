#include "MaterialPostProcess.h"
#include "resources/ResourceManager.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void MaterialPostProcess::Create(vk::RenderPass& renderPass, uint32_t subpass)
{
    m_pPipeline = PipelineBase::Builder().
    setCulling(vk::CullModeFlagBits::eFront).
    addShaderStage("../../assets/shaders/main/screenspace.vert").
    addShaderStage("../../assets/shaders/postprocess/bloomcompose.frag").
    build(renderPass, subpass);
    MaterialBase::Create(renderPass, subpass);
}

void MaterialPostProcess::ReCreate()
{
    MaterialBase::ReCreate();
}

void MaterialPostProcess::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->Clear();
    m_pDescriptorSet->Set("FBloomUbo", uboDesc);
    for(auto& [key, texture] : m_mTextures)
        m_pDescriptorSet->Set(key, texture); //"samplerColor", "samplerBrightness"
    MaterialBase::Update(uboDesc, imageIndex);
}

void MaterialPostProcess::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    MaterialBase::Bind(commandBuffer, imageIndex);
}

void MaterialPostProcess::Cleanup()
{
    MaterialBase::Cleanup();
}