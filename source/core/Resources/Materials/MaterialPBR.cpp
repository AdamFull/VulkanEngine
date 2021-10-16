#include "MaterialPBR.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/Pipeline/PipelineManager.h"

namespace Engine
{
    void MaterialPBR::Create()
    {
        MaterialBase::Create();

        auto images = USwapChain->GetImages().size();
        CreateDescriptorSets(images);

        auto pso = m_mPSO.at(GetShaderSet());
        FPipelineCreateInfo createInfo = PipelineConfig::CreatePBRPipeline(UDevice->GetSamples(), UPMGR_PL, UPMGR_PC);
        pso->pPipeline = PipelineFactory::CreatePipeline(createInfo, UDevice, USwapChain);
    }

    void MaterialPBR::ReCreate()
    {
        MaterialBase::ReCreate();

        auto images = USwapChain->GetImages().size();
        CreateDescriptorSets(images);

        auto pso = m_mPSO.at(GetShaderSet());
        FPipelineCreateInfo createInfo = PipelineConfig::CreatePBRPipeline(UDevice->GetSamples(), UPMGR_PL, UPMGR_PC);
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

    void MaterialPBR::CreateDescriptorSets(uint32_t images)
    {
        MaterialBase::CreateDescriptorSets(images);
    }
}