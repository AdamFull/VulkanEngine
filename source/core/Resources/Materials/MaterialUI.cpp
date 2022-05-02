#include "MaterialUI.h"
#include "graphics/overlay/ImguiOverlay.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"

using namespace Engine::Core;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void MaterialUI::Create(vk::RenderPass& renderPass, uint32_t subpass)
{
    m_pPipeline = CPipelineBase::Builder().
    setVertexInput(CVertexInput(VertexUI::getBindingDescription(), VertexUI::getAttributeDescriptions())).
    addShaderStage("../../assets/shaders/main/ui.vert").
    addShaderStage("../../assets/shaders/main/ui.frag").
    build(renderPass, subpass);
    MaterialBase::Create(renderPass, subpass);
}

void MaterialUI::ReCreate()
{
    m_pPipeline->recreatePipeline();
    MaterialBase::ReCreate();
}

void MaterialUI::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->clear();
    m_pDescriptorSet->set("FUniformDataUI", uboDesc);
    for(auto& [key, texture] : m_mTextures)
        m_pDescriptorSet->set(key, texture); //"fontSampler"
    MaterialBase::Update(uboDesc, imageIndex);
}

void MaterialUI::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    MaterialBase::Bind(commandBuffer, imageIndex);
}

void MaterialUI::Cleanup()
{
    MaterialBase::Cleanup();
}