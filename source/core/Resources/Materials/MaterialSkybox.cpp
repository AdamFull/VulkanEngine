#include "MaterialSkybox.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"

using namespace Engine::Core;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void CMaterialSkybox::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    m_pPipeline = CPipelineBase::Builder().
    setVertexInput(CVertexInput(Vertex::getBindingDescription(), Vertex::getAttributeDescriptions())).
    setCulling(vk::CullModeFlagBits::eFront).
    setColorAttachments(7).
    addShaderStage("../../assets/shaders/main/skybox.vert").
    addShaderStage("../../assets/shaders/main/skybox.frag").
    build(renderPass, subpass);
    CMaterialBase::create(renderPass, subpass);
}

void CMaterialSkybox::reCreate()
{
    m_pPipeline->recreatePipeline();
    CMaterialBase::reCreate();
}

void CMaterialSkybox::update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->clear();
    m_pDescriptorSet->set("FUniformData", uboDesc);
    for(auto& [key, texture] : m_mTextures)
        m_pDescriptorSet->set(key, texture); //"samplerCubeMap"
    CMaterialBase::update(uboDesc, imageIndex);
}

void CMaterialSkybox::bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    CMaterialBase::bind(commandBuffer, imageIndex);
}

void CMaterialSkybox::cleanup()
{
    CMaterialBase::cleanup();
}