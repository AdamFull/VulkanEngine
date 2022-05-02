#include "MaterialBlur.h"
#include "resources/ResourceManager.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void CMaterialBlur::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    m_pPipeline = CPipelineBase::Builder().
    setCulling(vk::CullModeFlagBits::eFront).
    addShaderStage("../../assets/shaders/main/screenspace.vert").
    addShaderStage("../../assets/shaders/postprocess/gaussianblur.frag").
    build(renderPass, subpass);
    CMaterialBase::create(renderPass, subpass);
}

void CMaterialBlur::reCreate()
{
    m_pPipeline->recreatePipeline();
    CMaterialBase::reCreate();
}

void CMaterialBlur::update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->clear();
    m_pDescriptorSet->set("FBloomUbo", uboDesc);
    for(auto& [key, texture] : m_mTextures)
        m_pDescriptorSet->set(key, texture); // "samplerColor"
    CMaterialBase::update(uboDesc, imageIndex);
}

void CMaterialBlur::bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    CMaterialBase::bind(commandBuffer, imageIndex);
}

void CMaterialBlur::cleanup()
{
    CMaterialBase::cleanup();
}