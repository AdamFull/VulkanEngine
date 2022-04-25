#include "MaterialSkybox.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/rendering/RendererBase.h"
#include "graphics/rendering/RendererBase.h"
#include "resources/ResourceManager.h"

using namespace Engine::Core;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void MaterialSkybox::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::eDeferredPBR)->GetRenderPass();
    m_pPipeline = PipelineBase::Builder().
    SetVertexInput(VertexInput(Vertex::getBindingDescription(), Vertex::getAttributeDescriptions())).
    SetRenderPass(renderPass).
    SetCulling(vk::CullModeFlagBits::eFront).
    SetColorAttachments(6).
    AddShaderStage("../../assets/shaders/main/skybox.vert").
    AddShaderStage("../../assets/shaders/main/skybox.frag").
    Build();
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