#include "MaterialUI.h"
#include "Core/Overlay/ImguiOverlay.h"
#include "Core/VulkanHighLevel.h"
#include "Resources/ResourceManager.h"

using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;

void MaterialUI::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    initial.vertexInputDesc = Core::VertexUI::getBindingDescription();
    initial.vertexAtribDesc = Core::VertexUI::getAttributeDescriptions();
    initial.shaders = 
    {
        {"../../assets/shaders/main/ui.vert"},
        {"../../assets/shaders/main/ui.frag"}
    };
    renderPass = USwapChain->GetRenderPass();
    MaterialBase::Create(pResMgr);
}

void MaterialUI::ReCreate()
{
    renderPass = USwapChain->GetRenderPass();
    MaterialBase::ReCreate();
}

void MaterialUI::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->Clear();
    m_pDescriptorSet->Set("FUniformDataUI", uboDesc);
    m_pDescriptorSet->Set("fontSampler", m_mTextures[ETextureAttachmentType::eDiffuseAlbedo]);
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