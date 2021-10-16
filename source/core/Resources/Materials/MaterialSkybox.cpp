#include "MaterialSkybox.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/Pipeline/PipelineManager.h"

namespace Engine
{
    void MaterialSkybox::Create()
    {
        MaterialBase::Create();

        auto images = USwapChain->GetImages().size();
        CreateDescriptorSets(images);

        auto pso = m_mPSO.at(GetShaderSet());
        FPipelineCreateInfo skyboxInfo = PipelineConfig::CreateSkyboxPipeline(UDevice->GetSamples(), UPMGR_PL, UPMGR_PC);
        pso->pPipeline = PipelineFactory::CreatePipeline(skyboxInfo, UDevice, USwapChain);
    }

    void MaterialSkybox::ReCreate()
    {
        MaterialBase::ReCreate();

        auto images = USwapChain->GetImages().size();
        CreateDescriptorSets(images);

        auto pso = m_mPSO.at(GetShaderSet());
        FPipelineCreateInfo skyboxInfo = PipelineConfig::CreateSkyboxPipeline(UDevice->GetSamples(), UPMGR_PL, UPMGR_PC);
        pso->pPipeline->RecreatePipeline(skyboxInfo, UDevice, USwapChain);
    }

    void MaterialSkybox::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
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

        std::array<vk::WriteDescriptorSet, 2> descriptorWrites{};
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
        descriptorWrites[1].pImageInfo = &m_mTextures[ETextureAttachmentType::eCubemap]->GetDescriptor();

        UDevice->GetLogical()->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    void MaterialSkybox::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        MaterialBase::Bind(commandBuffer, imageIndex);
    }

    void MaterialSkybox::Cleanup()
    {
        MaterialBase::Cleanup();
    }

    void MaterialSkybox::Destroy()
    {
        MaterialBase::Destroy();
    }

    void MaterialSkybox::CreateDescriptorSets(uint32_t images)
    {
        MaterialBase::CreateDescriptorSets(images);
    }
}