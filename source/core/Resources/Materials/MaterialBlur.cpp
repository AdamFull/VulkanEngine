#include "MaterialBlur.h"
#include "Core/VulkanHighLevel.h"
#include "Resources/ResourceManager.h"
#include "Core/Rendering/RendererBase.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void MaterialBlur::Create(std::shared_ptr<ResourceManager> pResMgr, vk::RenderPass& rPass)
{
    m_pPipeline = PipelineBase::Builder().
    SetRenderPass(rPass).
    SetCulling(vk::CullModeFlagBits::eFront).
    AddShaderStage("../../assets/shaders/main/screenspace.vert").
    AddShaderStage("../../assets/shaders/postprocess/gaussianblur.frag").
    Build();
    MaterialBase::Create(pResMgr, rPass);
}

void MaterialBlur::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::ePostProcess)->GetRenderPass();
    m_pPipeline = PipelineBase::Builder().
    SetRenderPass(renderPass).
    SetCulling(vk::CullModeFlagBits::eFront).
    AddShaderStage("../../assets/shaders/main/screenspace.vert").
    AddShaderStage("../../assets/shaders/postprocess/gaussianblur.frag").
    Build();
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