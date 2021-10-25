#include "MaterialUI.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanDevice.h"

#include "Renderer/Descriptor/DescriptorSetLayout.h"
#include "Renderer/Descriptor/DescriptorSet.h"
#include "Renderer/Descriptor/DescriptorWriter.h"

namespace Engine
{
    void MaterialUI::Create(std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        MaterialBase::Create(pUniformBuffer);
    }

    void MaterialUI::ReCreate()
    {
        MaterialBase::ReCreate();
    }

    void MaterialUI::Update(uint32_t imageIndex)
    {
        MaterialBase::Update(imageIndex);

        m_pMatWriter->Update(UDevice, imageIndex);
        m_pTexWriter->Update(UDevice, imageIndex);
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

    void MaterialUI::CreateDescriptors(uint32_t images, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        // Matrices uniform
        auto matSetLayout = VulkanDescriptorSetLayout::Builder().
        addBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex).build(UDevice);

        auto matSet = std::make_unique<VulkanDescriptorSet>();
        matSet->Create(UDevice, m_pDescriptorPool, matSetLayout, images);

        m_pMatWriter = std::make_unique<VulkanDescriptorWriter>();
        m_pMatWriter->Create(std::move(matSetLayout), m_pDescriptorPool, std::move(matSet));

         vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = pUniformBuffer->GetBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(FUniformDataUI);
        
        m_pMatWriter->WriteBuffer(0, &bufferInfo);
        
        // Texture uniform
        auto texSetLayout = VulkanDescriptorSetLayout::Builder().
        addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).
        build(UDevice);

        auto texSet = std::make_unique<VulkanDescriptorSet>();
        matSet->Create(UDevice, m_pDescriptorPool, texSetLayout, images);

        m_pTexWriter = std::make_unique<VulkanDescriptorWriter>();
        m_pTexWriter->Create(std::move(texSetLayout), m_pDescriptorPool, std::move(texSet));

        m_pTexWriter->WriteImage(0, &m_mTextures[ETextureAttachmentType::eColor]->GetDescriptor());
    }
}