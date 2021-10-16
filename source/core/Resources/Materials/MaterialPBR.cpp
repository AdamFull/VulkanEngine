#include "MaterialPBR.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanDevice.h"

namespace Engine
{
    void MaterialPBR::Create()
    {
        MaterialBase::Create();

        auto images = USwapChain->GetImages().size();
        CreateDescriptorSetLayout();
        CreateDescriptorPool(images);
        CreateDescriptorSets(images);
        CreatePipelineLayout(images);

        auto pso = m_mPSO.at(GetShaderSet());
        FPipelineCreateInfo createInfo = PipelineConfig::CreatePBRPipeline(UDevice->GetSamples(), pso->pipelineLayout, pso->pipelineCache);
        pso->pPipeline = PipelineFactory::CreatePipeline(createInfo, UDevice, USwapChain);
    }

    void MaterialPBR::ReCreate()
    {
        MaterialBase::ReCreate();

        auto images = USwapChain->GetImages().size();
        CreateDescriptorSetLayout();
        CreateDescriptorPool(images);
        CreateDescriptorSets(images);
        CreatePipelineCache();
        CreatePipelineLayout(images);

        auto pso = m_mPSO.at(GetShaderSet());
        FPipelineCreateInfo createInfo = PipelineConfig::CreatePBRPipeline(UDevice->GetSamples(), pso->pipelineLayout, pso->pipelineCache);
        pso->pPipeline->RecreatePipeline(createInfo, UDevice, USwapChain);
    }

    void MaterialPBR::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        MaterialBase::Update(imageIndex, pUniformBuffer);

        auto pso = m_mPSO.at(GetShaderSet());
        auto& uniformBuffer = pUniformBuffer->GetBuffer();
        auto& descriptorSet = pso->vDescriptorSets[imageIndex];

        vk::DescriptorBufferInfo bufferInfo{};
        //GetCurrentUniform
        bufferInfo.buffer = uniformBuffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(FUniformData);

        std::array<vk::WriteDescriptorSet, 6> descriptorWrites{};
        descriptorWrites[0].dstSet = descriptorSet;
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].dstSet = descriptorSet;
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &m_mTextures[ETextureAttachmentType::eColor]->GetDescriptor();

        descriptorWrites[2].dstSet = descriptorSet;
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pImageInfo = &m_mTextures[ETextureAttachmentType::eNormal]->GetDescriptor();

        descriptorWrites[3].dstSet = descriptorSet;
        descriptorWrites[3].dstBinding = 3;
        descriptorWrites[3].dstArrayElement = 0;
        descriptorWrites[3].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrites[3].descriptorCount = 1;
        descriptorWrites[3].pImageInfo = &m_mTextures[ETextureAttachmentType::eSpecular]->GetDescriptor();

        descriptorWrites[4].dstSet = descriptorSet;
        descriptorWrites[4].dstBinding = 4;
        descriptorWrites[4].dstArrayElement = 0;
        descriptorWrites[4].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrites[4].descriptorCount = 1;
        descriptorWrites[4].pImageInfo = &m_mTextures[ETextureAttachmentType::eAlbedo]->GetDescriptor();

        descriptorWrites[5].dstSet = descriptorSet;
        descriptorWrites[5].dstBinding = 5;
        descriptorWrites[5].dstArrayElement = 0;
        descriptorWrites[5].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrites[5].descriptorCount = 1;
        descriptorWrites[5].pImageInfo = &m_mTextures[ETextureAttachmentType::eOcclusion]->GetDescriptor();

        UDevice->GetLogical()->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
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

    void MaterialPBR::CreateDescriptorSetLayout()
    {
        MaterialBase::CreateDescriptorSetLayout();

        auto pso = m_mPSO.at(GetShaderSet());
        vk::DescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

        vk::DescriptorSetLayoutBinding diffuseLayoutBinding{};
        diffuseLayoutBinding.binding = 1;
        diffuseLayoutBinding.descriptorCount = 1;
        diffuseLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        diffuseLayoutBinding.pImmutableSamplers = nullptr;
        diffuseLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        vk::DescriptorSetLayoutBinding normalLayoutBinding{};
        normalLayoutBinding.binding = 2;
        normalLayoutBinding.descriptorCount = 1;
        normalLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        normalLayoutBinding.pImmutableSamplers = nullptr;
        normalLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        vk::DescriptorSetLayoutBinding specularLayoutBinding{};
        specularLayoutBinding.binding = 3;
        specularLayoutBinding.descriptorCount = 1;
        specularLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        specularLayoutBinding.pImmutableSamplers = nullptr;
        specularLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        vk::DescriptorSetLayoutBinding albedoLayoutBinding{};
        albedoLayoutBinding.binding = 4;
        albedoLayoutBinding.descriptorCount = 1;
        albedoLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        albedoLayoutBinding.pImmutableSamplers = nullptr;
        albedoLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        vk::DescriptorSetLayoutBinding aoLayoutBinding{};
        aoLayoutBinding.binding = 5;
        aoLayoutBinding.descriptorCount = 1;
        aoLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        aoLayoutBinding.pImmutableSamplers = nullptr;
        aoLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        std::array<vk::DescriptorSetLayoutBinding, 6> bindings = {uboLayoutBinding, diffuseLayoutBinding, normalLayoutBinding, specularLayoutBinding, albedoLayoutBinding, aoLayoutBinding};
        vk::DescriptorSetLayoutCreateInfo createInfo{};
        createInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
        createInfo.pBindings = bindings.data();

        //TODO: check result
        auto result = UDevice->GetLogical()->createDescriptorSetLayout(&createInfo, nullptr, &pso->descriptorSetLayout);
    }

    void MaterialPBR::CreateDescriptorPool(uint32_t images)
    {
        MaterialBase::CreateDescriptorPool(images);

        auto pso = m_mPSO.at(GetShaderSet());
        std::array<vk::DescriptorPoolSize, 6> poolSizes{};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = images;
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = 1;
        poolSizes[2].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[2].descriptorCount = 1;
        poolSizes[3].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[3].descriptorCount = 1;
        poolSizes[4].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[4].descriptorCount = 1;
        poolSizes[5].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[5].descriptorCount = 1;

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = images;

        //TODO:: add result checking
        pso->descriptorPool = UDevice->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(poolInfo);
    }

    void MaterialPBR::CreateDescriptorSets(uint32_t images)
    {
        MaterialBase::CreateDescriptorSets(images);
    }

    void MaterialPBR::CreatePipelineCache()
    {
        MaterialBase::CreatePipelineCache();
    }

    void MaterialPBR::CreatePipelineLayout(uint32_t images)
    {
        MaterialBase::CreatePipelineLayout(images);
    }
}