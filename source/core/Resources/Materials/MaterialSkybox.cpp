#include "MaterialSkybox.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"

using namespace Engine::Core;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void MaterialSkybox::Create(vk::RenderPass& renderPass, uint32_t subpass)
{
    m_pPipeline = PipelineBase::Builder().
    setVertexInput(VertexInput(Vertex::getBindingDescription(), Vertex::getAttributeDescriptions())).
    setCulling(vk::CullModeFlagBits::eFront).
    setColorAttachments(7).
    addShaderStage("../../assets/shaders/main/skybox.vert").
    addShaderStage("../../assets/shaders/main/skybox.frag").
    build(renderPass, subpass);
    MaterialBase::Create(renderPass, subpass);
}

void MaterialSkybox::ReCreate()
{
    m_pPipeline->RecreatePipeline();
    MaterialBase::ReCreate();
}

void MaterialSkybox::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->Clear();
    m_pDescriptorSet->Set("FUniformData", uboDesc);
    for(auto& [key, texture] : m_mTextures)
        m_pDescriptorSet->Set(key, texture); //"samplerCubeMap"
    MaterialBase::Update(uboDesc, imageIndex);
}

void MaterialSkybox::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    MaterialBase::Bind(commandBuffer, imageIndex);
}

void MaterialSkybox::Cleanup()
{
    MaterialBase::Cleanup();
}