#include "MaterialDiffuse.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void CMaterialDiffuse::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    m_pPipeline = CPipelineBase::Builder().
    setVertexInput(CVertexInput(Vertex::getBindingDescription(), Vertex::getAttributeDescriptions())).
    setCulling(vk::CullModeFlagBits::eBack).
    setColorAttachments(7).
    setDepthEnabled(VK_TRUE).
    addShaderStage("../../assets/shaders/main/diffuse.vert").
    addShaderStage("../../assets/shaders/main/diffuse.frag").
    build(renderPass, subpass);
    CMaterialBase::create(renderPass, subpass);
}

void CMaterialDiffuse::reCreate()
{
    m_pPipeline->recreatePipeline();
    CMaterialBase::reCreate();
}

void CMaterialDiffuse::update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->clear();
    m_pDescriptorSet->set("FUniformData", uboDesc);
    for(auto& [key, texture] : m_mTextures)
        m_pDescriptorSet->set(key, texture); // "color_tex", "metalRough_tex", "emissive_tex", "normal_tex", "height_tex", "ao_tex"
    CMaterialBase::update(uboDesc, imageIndex);
}

void CMaterialDiffuse::bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    CMaterialBase::bind(commandBuffer, imageIndex);
}

void CMaterialDiffuse::cleanup()
{
    CMaterialBase::cleanup();
}