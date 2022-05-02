#include "MaterialDeferred.h"
#include "resources/ResourceManager.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void CMaterialDeferred::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    m_pPipeline = CPipelineBase::Builder().
    setCulling(vk::CullModeFlagBits::eFront).
    setColorAttachments(2).
    addShaderStage("../../assets/shaders/main/screenspace.vert").
    addShaderStage("../../assets/shaders/main/deferred.frag").
    build(renderPass, subpass);
    CMaterialBase::create(renderPass, subpass);
}

void CMaterialDeferred::reCreate()
{
    m_pPipeline->recreatePipeline();
    CMaterialBase::reCreate();
}

void CMaterialDeferred::update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->clear();
    m_pDescriptorSet->set("UBOLightning", uboDesc);
    for(auto& [key, texture] : m_mTextures)
        m_pDescriptorSet->set(key, texture); //"brdflut_tex", "irradiance_tex", "prefiltred_tex", "position_tex", "lightning_mask_tex", "normal_tex", "albedo_tex", "emission_tex", "mrah_tex"
    CMaterialBase::update(uboDesc, imageIndex);
}

void CMaterialDeferred::bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    CMaterialBase::bind(commandBuffer, imageIndex);
}

void CMaterialDeferred::cleanup()
{
    CMaterialBase::cleanup();
}