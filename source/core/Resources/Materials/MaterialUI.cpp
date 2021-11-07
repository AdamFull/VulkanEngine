#include "MaterialUI.h"
#include "Core/Overlay/ImguiOverlay.h"
#include "Core/VulkanAllocator.h"

using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;

void MaterialUI::Create()
{
    renderPass = m_swapchain->GetRenderPass();
    MaterialBase::Create();
}

void MaterialUI::ReCreate()
{
    MaterialBase::ReCreate();
}

void MaterialUI::Update(uint32_t imageIndex)
{
    MaterialBase::Update(imageIndex);

    auto bufferInfo = VulkanDescriptorWriter().
    WriteBuffer(0, m_pMatDesc->GetSetLayout(0)->GetBindings(), &UOverlay->GetBuffer(imageIndex)->GetDscriptor()).
    Build();
    m_pMatDesc->Update(0, imageIndex, bufferInfo);

    auto imageInfo = VulkanDescriptorWriter().
    WriteImage(0, m_pMatDesc->GetSetLayout(1)->GetBindings(), &m_mTextures[ETextureAttachmentType::eDiffuseAlbedo]->GetDescriptor()).
    Build();
    m_pMatDesc->Update(1, imageIndex, imageInfo);
}

void MaterialUI::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    MaterialBase::Bind(commandBuffer, imageIndex);
}

void MaterialUI::Cleanup()
{
    MaterialBase::Cleanup();
}

void MaterialUI::Destroy()
{
    MaterialBase::Destroy();
}

void MaterialUI::CreateDescriptors(uint32_t images)
{
    // Matrices uniform
    auto matSetLayout = VulkanDescriptorSetLayout::Builder().
    addBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex).
    build();

    auto matSet = Core::FDefaultAllocator::Allocate<VulkanDescriptorSet>();
    matSet->Create(m_pDescriptorPool, matSetLayout, images);

    m_pMatDesc->AttachDescriptorSet(std::move(matSet), std::move(matSetLayout));

    // Texture uniform
    auto texSetLayout = VulkanDescriptorSetLayout::Builder().addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).build();

    auto texSet = Core::FDefaultAllocator::Allocate<VulkanDescriptorSet>();
    texSet->Create(m_pDescriptorPool, texSetLayout, images);

    m_pMatDesc->AttachDescriptorSet(std::move(texSet), std::move(texSetLayout));
}