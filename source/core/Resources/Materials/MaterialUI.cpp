#include "MaterialUI.h"
#include "graphics/overlay/ImguiOverlay.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"

using namespace Engine::Core;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void CMaterialUI::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    m_pPipeline = CPipelineBase::Builder().
    setVertexInput(CVertexInput(VertexUI::getBindingDescription(), VertexUI::getAttributeDescriptions())).
    addShaderStage("../../assets/shaders/main/ui.vert").
    addShaderStage("../../assets/shaders/main/ui.frag").
    build(renderPass, subpass);
    CMaterialBase::create(renderPass, subpass);
}

void CMaterialUI::reCreate()
{
    m_pPipeline->recreatePipeline();
    CMaterialBase::reCreate();
}

void CMaterialUI::update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->clear();
    m_pDescriptorSet->set("FUniformDataUI", uboDesc);
    for(auto& [key, texture] : m_mTextures)
        m_pDescriptorSet->set(key, texture); //"fontSampler"
    CMaterialBase::update(uboDesc, imageIndex);
}

void CMaterialUI::bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    CMaterialBase::bind(commandBuffer, imageIndex);
}

void CMaterialUI::cleanup()
{
    CMaterialBase::cleanup();
}