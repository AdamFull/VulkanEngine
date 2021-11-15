#include "MaterialSkybox.h"
#include "Core/VulkanAllocator.h"
#include "Resources/ResourceManager.h"

using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;

MaterialSkybox::MaterialSkybox(std::shared_ptr<Core::Device> device, std::shared_ptr<Core::SwapChain> swapchain)
{
    m_device = device;
    m_swapchain = swapchain;
}

void MaterialSkybox::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    renderPass = m_swapchain->GetOffscreenRenderPass();

    //m_mTextures[ETextureAttachmentType::eCubemap] = pResMgr->Get<Texture::TextureBase>("environment_component_prefiltred_cube");

    MaterialBase::Create(pResMgr);
}

void MaterialSkybox::ReCreate()
{
    renderPass = m_swapchain->GetOffscreenRenderPass();
    MaterialBase::ReCreate();
}

void MaterialSkybox::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    MaterialBase::Update(uboDesc, imageIndex);

    auto bufferInfo = VulkanDescriptorWriter().
    WriteBuffer(0, m_pMatDesc->GetSetLayout(0)->GetBindings(), &uboDesc).
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