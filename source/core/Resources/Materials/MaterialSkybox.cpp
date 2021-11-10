#include "MaterialSkybox.h"
#include "Core/VulkanAllocator.h"

using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;

MaterialSkybox::MaterialSkybox(std::shared_ptr<Core::Device> device, std::shared_ptr<Core::SwapChain> swapchain)
{
    m_device = device;
    m_swapchain = swapchain;
}

void MaterialSkybox::Create()
{
    renderPass = m_swapchain->GetRenderPass();
    MaterialBase::Create();
}

void MaterialSkybox::ReCreate()
{
    renderPass = m_swapchain->GetRenderPass();
    MaterialBase::ReCreate();
}

void MaterialSkybox::Update(uint32_t imageIndex)
{
    MaterialBase::Update(imageIndex);

    auto bufferInfo = VulkanDescriptorWriter().
    WriteBuffer(0, m_pMatDesc->GetSetLayout(0)->GetBindings(), &UUniform->GetUniformBuffer(imageIndex)->GetDscriptor()).
    Build();
    m_pMatDesc->Update(0, imageIndex, bufferInfo);

    auto imageInfo = VulkanDescriptorWriter().
    WriteImage(0, m_pMatDesc->GetSetLayout(1)->GetBindings(), &m_mTextures[ETextureAttachmentType::eCubemap]->GetDescriptor()).
    Build();
    m_pMatDesc->Update(1, imageIndex, imageInfo);
}

void MaterialSkybox::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    MaterialBase::Bind(commandBuffer, imageIndex);
}

void MaterialSkybox::Cleanup()
{
    MaterialBase::Cleanup();
}

void MaterialSkybox::CreateDescriptors(uint32_t images)
{
    // Matrices uniform
    auto matSetLayout = VulkanDescriptorSetLayout::Builder().
    addBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex).
    build();

    auto matSet = Core::FDefaultAllocator::Allocate<VulkanDescriptorSet>();
    matSet->Create(m_pDescriptorPool, matSetLayout, images);

    m_pMatDesc->AttachDescriptorSet(std::move(matSet), std::move(matSetLayout));

    // Texture uniform
    auto texSetLayout = VulkanDescriptorSetLayout::Builder().
    addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).
    build();

    auto texSet = Core::FDefaultAllocator::Allocate<VulkanDescriptorSet>();
    texSet->Create(m_pDescriptorPool, texSetLayout, images);

    m_pMatDesc->AttachDescriptorSet(std::move(texSet), std::move(texSetLayout));
}