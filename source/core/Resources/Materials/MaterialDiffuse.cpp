#include "MaterialDiffuse.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanDevice.h"

namespace Engine
{
    void MaterialDiffuse::Create()
    {
        MaterialBase::Create();

        auto images = USwapChain->GetImages().size();
        CreateDescriptorSets(images);
    }

    void MaterialDiffuse::ReCreate()
    {
        MaterialBase::ReCreate();

        auto images = USwapChain->GetImages().size();
        CreateDescriptorSets(images);
    }

    void MaterialDiffuse::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        MaterialBase::Update(imageIndex, pUniformBuffer);

        auto& uniformBuffer = pUniformBuffer->GetBuffer();

        vk::DescriptorBufferInfo bufferInfo{};
        //GetCurrentUniform
        bufferInfo.buffer = uniformBuffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(FUniformData);

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
        descriptorWrites.resize(3);
        for(uint32_t i = 0; i < 3; i++)
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

    void MaterialDiffuse::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        MaterialBase::Bind(commandBuffer, imageIndex);
    }

    void MaterialDiffuse::Cleanup()
    {
        MaterialBase::Cleanup();
    }

    void MaterialDiffuse::Destroy()
    {
        MaterialBase::Destroy();
    }

    void MaterialDiffuse::CreateDescriptorSets(uint32_t images)
    {
        MaterialBase::CreateDescriptorSets(images);
    }

    void MaterialDiffuse::CreateDescriptorSetLayout()
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

       std::vector<vk::DescriptorSetLayoutBinding> vBindings;
        for(size_t i = 0; i < 4; i++)
        {
            vk::DescriptorSetLayoutBinding binding;
            binding.binding = i;
            binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
            binding.descriptorCount = 1;
            binding.pImmutableSamplers = nullptr;
            binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
            vBindings.emplace_back(binding);
        }
        
        vk::DescriptorSetLayoutCreateInfo texturesCreateInfo{};
        texturesCreateInfo.bindingCount = static_cast<uint32_t>(vBindings.size());;
        texturesCreateInfo.pBindings = vBindings.data();

        result = UDevice->GetLogical()->createDescriptorSetLayout(&texturesCreateInfo, nullptr, &descriptors.texturesSetLayout);
    }
}