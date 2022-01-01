#include "MaterialSkybox.h"
#include "Core/VulkanHighLevel.h"
#include "Core/Rendering/RendererBase.h"
#include "Resources/ResourceManager.h"
#include "Core/Rendering/RendererBase.h"

using namespace Engine::Core;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;

void MaterialSkybox::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    initial.vertexInputDesc = Vertex::getBindingDescription();
    initial.vertexAtribDesc = Vertex::getAttributeDescriptions();
    initial.culling = vk::CullModeFlagBits::eFront;
    initial.color_attachments = 6;
    initial.shaders = 
    {
        {"../../assets/shaders/main/skybox.vert"},
        {"../../assets/shaders/main/skybox.frag"}
    };
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::eDeferredPBR)->GetRenderPass();
    MaterialBase::Create(pResMgr);
}

void MaterialSkybox::ReCreate()
{
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::eDeferredPBR)->GetRenderPass();
    MaterialBase::ReCreate();
}

void MaterialSkybox::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->Clear();
    m_pDescriptorSet->Set("FUniformData", uboDesc);
    m_pDescriptorSet->Set("samplerCubeMap", m_mTextures[ETextureAttachmentType::eCubemap]);
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