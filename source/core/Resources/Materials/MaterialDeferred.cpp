#include "MaterialDeferred.h"
#include "Core/VulkanHighLevel.h"
#include "Resources/ResourceManager.h"
#include "Core/Rendering/RendererBase.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;

void MaterialDeferred::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    initial.culling = vk::CullModeFlagBits::eFront;
    initial.color_attachments = 2;
    initial.shaders = 
    {
        {"../../assets/shaders/main/screenspace.vert"},
        {"../../assets/shaders/main/deferred.frag"}
    };

    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::ePBRComposition)->GetRenderPass();
    MaterialBase::Create(pResMgr);
}

void MaterialDeferred::ReCreate()
{
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::ePBRComposition)->GetRenderPass();
    MaterialBase::ReCreate();
}

void MaterialDeferred::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->Clear();
    m_pDescriptorSet->Set("UBOLightning", uboDesc);
    m_pDescriptorSet->Set("brdflut_tex", m_mTextures[ETextureAttachmentType::eBRDFLUT]);
    m_pDescriptorSet->Set("irradiance_tex", m_mTextures[ETextureAttachmentType::eIrradiance]);
    m_pDescriptorSet->Set("prefiltred_tex", m_mTextures[ETextureAttachmentType::ePrefiltred]);
    m_pDescriptorSet->Set("position_tex", m_mTextures[ETextureAttachmentType::ePosition]);
    m_pDescriptorSet->Set("lightning_mask_tex", m_mTextures[ETextureAttachmentType::eLightningMask]);
    m_pDescriptorSet->Set("normal_tex", m_mTextures[ETextureAttachmentType::eNormal]);
    m_pDescriptorSet->Set("albedo_tex", m_mTextures[ETextureAttachmentType::eDiffuseAlbedo]);
    m_pDescriptorSet->Set("emission_tex", m_mTextures[ETextureAttachmentType::eEmissive]);
    m_pDescriptorSet->Set("mrah_tex", m_mTextures[ETextureAttachmentType::eMRAH]);
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