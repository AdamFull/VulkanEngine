#include "MaterialPBR.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanDevice.h"

namespace Engine
{
    void MaterialPBR::Create(std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        MaterialBase::Create(pUniformBuffer);
    }

    void MaterialPBR::ReCreate()
    {
        MaterialBase::ReCreate();
    }

    void MaterialPBR::Update(uint32_t imageIndex)
    {
        MaterialBase::Update(imageIndex);
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

    void MaterialPBR::CreateDescriptors(uint32_t images, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
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
        bufferInfo.range = sizeof(FUniformData);
        
        m_pMatWriter->WriteBuffer(0, &bufferInfo);
        
        // Texture uniform
        auto texSetLayout = VulkanDescriptorSetLayout::Builder().
        addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).
        addBinding(1, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).
        addBinding(2, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).
        build(UDevice);

        auto texSet = std::make_unique<VulkanDescriptorSet>();
        texSet->Create(UDevice, m_pDescriptorPool, texSetLayout, images);

        m_pTexWriter = std::make_unique<VulkanDescriptorWriter>();
        m_pTexWriter->Create(std::move(texSetLayout), m_pDescriptorPool, std::move(texSet));

        for(uint32_t i = 0; i < 3; i++)
        {
            auto binding = m_mTextureBindings[i];
            auto it = m_mTextures.find(binding);
            std::shared_ptr<TextureBase> texture;
            if(it != m_mTextures.end())
                texture = it->second;
            else
                texture = m_mTextures[ETextureAttachmentType::eEmpty];

            m_pTexWriter->WriteImage(i, &texture->GetDescriptor());
        }
    }
}