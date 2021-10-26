#include "MaterialPBR.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanDevice.h"

#include "Renderer/Descriptor/DescriptorSet.h"
#include "Renderer/Descriptor/DescriptorSetLayout.h"
#include "Renderer/Descriptor/DescriptorWriter.h"

namespace Engine
{
    void MaterialPBR::Create()
    {
        MaterialBase::Create();
    }

    void MaterialPBR::ReCreate()
    {
        MaterialBase::ReCreate();
    }

    void MaterialPBR::Update(uint32_t imageIndex)
    {
        MaterialBase::Update(imageIndex);

        auto bufferInfo = VulkanDescriptorWriter().
        WriteBuffer(0, m_pMatDesc->GetSetLayout(0)->GetBindings(), &UUniform->GetUniformBuffer(imageIndex)->GetDscriptor()).Build();
        m_pMatDesc->Update(0, imageIndex, bufferInfo);

        auto imageInfo = VulkanDescriptorWriter().
        WriteImage(0, m_pMatDesc->GetSetLayout(1)->GetBindings(), &m_mTextures[ETextureAttachmentType::eCubemap]->GetDescriptor()).Build();
        m_pMatDesc->Update(1, imageIndex, imageInfo);
    }

    void MaterialPBR::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        MaterialBase::Bind(commandBuffer, imageIndex);
    }

    void MaterialPBR::Cleanup()
    {
        MaterialBase::Cleanup();
    }

    void MaterialPBR::Destroy()
    {
        MaterialBase::Destroy();
    }

    void MaterialPBR::CreateDescriptors(uint32_t images)
    {
        // Matrices uniform
        auto matSetLayout = VulkanDescriptorSetLayout::Builder().
        addBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex).
        build(UDevice);

        auto matSet = std::make_unique<VulkanDescriptorSet>();
        matSet->Create(UDevice, m_pDescriptorPool, matSetLayout, images);

        m_pMatDesc->AttachDescriptorSet(std::move(matSet), std::move(matSetLayout));

        // Texture uniform
        auto texSetLayout = VulkanDescriptorSetLayout::Builder().
        addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).
        build(UDevice);

        auto texSet = std::make_unique<VulkanDescriptorSet>();
        texSet->Create(UDevice, m_pDescriptorPool, texSetLayout, images);

        m_pMatDesc->AttachDescriptorSet(std::move(texSet), std::move(texSetLayout));
    }
}