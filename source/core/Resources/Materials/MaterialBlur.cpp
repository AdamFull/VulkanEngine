#include "MaterialBlur.h"
#include "Core/VulkanHighLevel.h"
#include "Resources/ResourceManager.h"
#include "Core/Rendering/RendererBase.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;

void MaterialBlur::Create(std::shared_ptr<ResourceManager> pResMgr, vk::RenderPass& rPass)
{
    initial.culling = vk::CullModeFlagBits::eFront;
    initial.shaders = 
    {
        {"../../assets/shaders/main/screenspace.vert"},
        {"../../assets/shaders/postprocess/gaussianblur.frag"}
    };
    MaterialBase::Create(pResMgr, rPass);
}

void MaterialBlur::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    initial.culling = vk::CullModeFlagBits::eFront;
    initial.shaders = 
    {
        {"../../assets/shaders/main/screenspace.vert"},
        {"../../assets/shaders/postprocess/gaussianblur.frag"}
    };
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::ePostProcess)->GetRenderPass();
    MaterialBase::Create(pResMgr);
}

void MaterialBlur::ReCreate()
{
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::ePostProcess)->GetRenderPass();
    MaterialBase::ReCreate();
}

void MaterialBlur::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->Clear();
    m_pDescriptorSet->Set("FBloomUbo", uboDesc);
    m_pDescriptorSet->Set("samplerColor", m_mTextures[ETextureAttachmentType::eDiffuseAlbedo]);
    MaterialBase::Update(uboDesc, imageIndex);
}

void MaterialBlur::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    MaterialBase::Bind(commandBuffer, imageIndex);
}

void MaterialBlur::Cleanup()
{
    MaterialBase::Cleanup();
}