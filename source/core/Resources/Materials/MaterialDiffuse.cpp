#include "MaterialDiffuse.h"
#include "Core/VulkanHighLevel.h"
#include "Core/Rendering/RendererBase.h"
#include "Resources/ResourceManager.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;

void MaterialDiffuse::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    initial.vertexInputDesc = Vertex::getBindingDescription();
    initial.vertexAtribDesc = Vertex::getAttributeDescriptions();
    initial.culling = vk::CullModeFlagBits::eBack;
    initial.color_attachments = 6;
    initial.enableDepth = VK_TRUE;
    initial.shaders = 
    {
        {"../../assets/shaders/main/diffuse.vert"},
        {"../../assets/shaders/main/diffuse.frag"}
    };
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::eDeferredPBR)->GetRenderPass();
    MaterialBase::Create(pResMgr);
}

void MaterialDiffuse::ReCreate()
{
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::eDeferredPBR)->GetRenderPass();
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