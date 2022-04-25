#include "MaterialUI.h"
#include "graphics/overlay/ImguiOverlay.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"

using namespace Engine::Core;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void MaterialUI::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    renderPass = USwapChain->GetRenderPass();
    m_pPipeline = PipelineBase::Builder().
    SetVertexInput(VertexInput(VertexUI::getBindingDescription(), VertexUI::getAttributeDescriptions())).
    SetRenderPass(renderPass).
    AddShaderStage("../../assets/shaders/main/ui.vert").
    AddShaderStage("../../assets/shaders/main/ui.frag").
    Build();
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