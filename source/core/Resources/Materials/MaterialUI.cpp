#include "MaterialUI.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanDevice.h"

namespace Engine
{
    void MaterialUI::Create()
    {
        MaterialBase::Create();
        CreateDescriptorSets(USwapChain->GetImages().size());
    }

    void MaterialUI::ReCreate()
    {
        MaterialBase::ReCreate();
        CreateDescriptorSets(USwapChain->GetImages().size());
    }

    void MaterialUI::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        MaterialBase::Update(imageIndex, pUniformBuffer);

        auto& uniformBuffer = pUniformBuffer->GetBuffer();

        vk::DescriptorBufferInfo bufferInfo{};
        //GetCurrentUniform
        bufferInfo.buffer = uniformBuffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(FUniformDataUI);

        std::vector<vk::WriteDescriptorSet> descriptorWrites{};
        descriptorWrites.resize(1);
        descriptorWrites[0].dstSet = descriptors.matricesSet[imageIndex];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        UDevice->GetLogical()->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

        descriptorWrites.clear();
        for(uint32_t i = 0; i < 1; i++)
        {
            auto binding = m_mTextureBindings[i];
            auto it = m_mTextures.find(binding);
            std::shared_ptr<TextureBase> texture;
            if(it != m_mTextures.end())
                texture = it->second;
            else
                texture = m_mTextures[ETextureAttachmentType::eEmpty];

            descriptorWrites[i].dstSet = descriptors.texturesSet[imageIndex];
            descriptorWrites[i].dstBinding = i;
            descriptorWrites[i].dstArrayElement = 0;
            descriptorWrites[i].descriptorType = vk::DescriptorType::eCombinedImageSampler;
            descriptorWrites[i].descriptorCount = 1;
            descriptorWrites[i].pImageInfo = &texture->GetDescriptor();
        }

        UDevice->GetLogical()->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
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

    void MaterialUI::CreateDescriptorSets(uint32_t images)
    {
        MaterialBase::CreateDescriptorSets(images);
    }

    void MaterialUI::CreateDescriptorSetLayout()
    {
        MaterialBase::CreateDescriptorSetLayout();
        vk::DescriptorSetLayoutBinding matricesBinding;
        matricesBinding.binding = 0;
        matricesBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
        matricesBinding.descriptorCount = 1;
        matricesBinding.pImmutableSamplers = nullptr;
        matricesBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

        vk::DescriptorSetLayoutCreateInfo matricesCreateInfo{};
        matricesCreateInfo.bindingCount = 1;
        matricesCreateInfo.pBindings = &matricesBinding;

        auto result = UDevice->GetLogical()->createDescriptorSetLayout(&matricesCreateInfo, nullptr, &descriptors.matricesSetLayout);

        /*vk::DescriptorSetLayoutBinding skinBinding;
        skinBinding.binding = 0;
        skinBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
        skinBinding.descriptorCount = 1;
        skinBinding.pImmutableSamplers = nullptr;
        skinBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

        vk::DescriptorSetLayoutCreateInfo skinsCreateInfo{};
        matricesCreateInfo.bindingCount = 1;
        matricesCreateInfo.pBindings = &skinBinding;

        auto result = UDevice->GetLogical()->createDescriptorSetLayout(&skinsCreateInfo, nullptr, &descriptors.skinsSetLayout);*/

        vk::DescriptorSetLayoutBinding texturesBinding;
        texturesBinding.binding = 0;
        texturesBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        texturesBinding.descriptorCount = 1;
        texturesBinding.pImmutableSamplers = nullptr;
        texturesBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        vk::DescriptorSetLayoutCreateInfo texturesCreateInfo{};
        texturesCreateInfo.bindingCount = 1;
        texturesCreateInfo.pBindings = &texturesBinding;

        result = UDevice->GetLogical()->createDescriptorSetLayout(&texturesCreateInfo, nullptr, &descriptors.texturesSetLayout);
    }
}