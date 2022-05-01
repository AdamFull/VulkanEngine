#include "MaterialDeferred.h"
#include "resources/ResourceManager.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void MaterialDeferred::Create(vk::RenderPass& renderPass, uint32_t subpass)
{
    m_pPipeline = CPipelineBase::Builder().
    setCulling(vk::CullModeFlagBits::eFront).
    setColorAttachments(2).
    addShaderStage("../../assets/shaders/main/screenspace.vert").
    addShaderStage("../../assets/shaders/main/deferred.frag").
    build(renderPass, subpass);
    MaterialBase::Create(renderPass, subpass);
}

void MaterialDeferred::ReCreate()
{
    m_pPipeline->recreatePipeline();
    MaterialBase::ReCreate();
}

void MaterialDeferred::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->Clear();
    m_pDescriptorSet->Set("UBOLightning", uboDesc);
    for(auto& [key, texture] : m_mTextures)
        m_pDescriptorSet->Set(key, texture); //"brdflut_tex", "irradiance_tex", "prefiltred_tex", "position_tex", "lightning_mask_tex", "normal_tex", "albedo_tex", "emission_tex", "mrah_tex"
    MaterialBase::Update(uboDesc, imageIndex);
}

void MaterialDeferred::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    MaterialBase::Bind(commandBuffer, imageIndex);
}

void MaterialDeferred::Cleanup()
{
    MaterialBase::Cleanup();
}