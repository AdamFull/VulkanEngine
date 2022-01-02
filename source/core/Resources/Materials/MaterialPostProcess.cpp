#include "MaterialPostProcess.h"
#include "Core/VulkanHighLevel.h"
#include "Resources/ResourceManager.h"
#include "Core/Rendering/RendererBase.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void MaterialPostProcess::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::ePostProcess)->GetRenderPass();
    m_pPipeline = PipelineBase::Builder().
    SetRenderPass(renderPass).
    SetCulling(vk::CullModeFlagBits::eFront).
    AddShaderStage("../../assets/shaders/main/screenspace.vert").
    AddShaderStage("../../assets/shaders/postprocess/bloomcompose.frag").
    Build();
    MaterialBase::Create(pResMgr);
}

void MaterialPostProcess::ReCreate()
{
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::ePostProcess)->GetRenderPass();
    MaterialBase::ReCreate();
}

void MaterialPostProcess::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->Clear();
    m_pDescriptorSet->Set("FBloomUbo", uboDesc);
    m_pDescriptorSet->Set("samplerColor", m_mTextures[ETextureAttachmentType::eDiffuseAlbedo]);
    m_pDescriptorSet->Set("samplerBrightness", m_mTextures[ETextureAttachmentType::eBrightness]);
    MaterialBase::Update(uboDesc, imageIndex);
}

void MaterialPostProcess::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    MaterialBase::Bind(commandBuffer, imageIndex);
}

void MaterialPostProcess::Cleanup()
{
    MaterialBase::Cleanup();
}