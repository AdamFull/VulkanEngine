#include "MaterialDiffuse.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void MaterialDiffuse::Create(vk::RenderPass& renderPass, uint32_t subpass)
{
    m_pPipeline = PipelineBase::Builder().
    setVertexInput(VertexInput(Vertex::getBindingDescription(), Vertex::getAttributeDescriptions())).
    setCulling(vk::CullModeFlagBits::eBack).
    setColorAttachments(7).
    setDepthEnabled(VK_TRUE).
    addShaderStage("../../assets/shaders/main/diffuse.vert").
    addShaderStage("../../assets/shaders/main/diffuse.frag").
    build(renderPass, subpass);
    MaterialBase::Create(renderPass, subpass);
}

void MaterialDiffuse::ReCreate()
{
    m_pPipeline->RecreatePipeline();
    MaterialBase::ReCreate();
}

void MaterialDiffuse::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->Clear();
    m_pDescriptorSet->Set("FUniformData", uboDesc);
    m_pDescriptorSet->Set("color_tex", m_mTextures[ETextureAttachmentType::eDiffuseAlbedo]);
    m_pDescriptorSet->Set("metalRough_tex", m_mTextures[ETextureAttachmentType::eMetalicRoughness]);
    m_pDescriptorSet->Set("emissive_tex", m_mTextures[ETextureAttachmentType::eEmissive]);
    m_pDescriptorSet->Set("normal_tex", m_mTextures[ETextureAttachmentType::eNormal]);
    m_pDescriptorSet->Set("height_tex", m_mTextures[ETextureAttachmentType::eHeight]);
    m_pDescriptorSet->Set("ao_tex", m_mTextures[ETextureAttachmentType::eOcclusion]);
    MaterialBase::Update(uboDesc, imageIndex);
}

void MaterialDiffuse::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    MaterialBase::Bind(commandBuffer, imageIndex);
}

void MaterialDiffuse::Cleanup()
{
    MaterialBase::Cleanup();
}