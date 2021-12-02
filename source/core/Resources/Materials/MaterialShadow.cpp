#include "MaterialShadow.h"
#include "Core/VulkanHighLevel.h"
#include "Core/Rendering/RendererBase.h"
#include "Resources/ResourceManager.h"
#include "Core/Rendering/RendererBase.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;

void MaterialShadow::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::eDeferredPBR)->GetRenderPass();
    MaterialBase::Create(pResMgr);
}

void MaterialShadow::ReCreate()
{
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::eDeferredPBR)->GetRenderPass();
    MaterialBase::ReCreate();
}

void MaterialShadow::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    MaterialBase::Update(uboDesc, imageIndex);

    auto bufferInfo = VulkanDescriptorWriter().
    WriteBuffer(0, m_pMatDesc->GetSetLayout(0)->GetBindings(), &uboDesc).
    Build();
    m_pMatDesc->Update(0, imageIndex, bufferInfo);
}

void MaterialShadow::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    MaterialBase::Bind(commandBuffer, imageIndex);
}

void MaterialShadow::Cleanup()
{
    MaterialBase::Cleanup();
}

void MaterialShadow::CreateDescriptors(uint32_t images)
{
    // Matrices uniform
    auto matSetLayout = VulkanDescriptorSetLayout::Builder().
    addBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eGeometry).
    build();

    auto matSet = std::make_unique<VulkanDescriptorSet>();
    matSet->Create(m_pDescriptorPool, matSetLayout, images);

    m_pMatDesc->AttachDescriptorSet(std::move(matSet), std::move(matSetLayout));
}
