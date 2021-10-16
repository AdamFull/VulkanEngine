#include "MaterialSkybox.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanDevice.h"

namespace Engine
{
    void MaterialSkybox::Create()
    {
        MaterialBase::Create();

        auto images = USwapChain->GetImages().size();
        CreateDescriptorSetLayout();
        CreateDescriptorPool(images);
        CreateDescriptorSets(images);
        CreatePipelineLayout(images);

        auto pso = m_mPSO.at(GetShaderSet());
        FPipelineCreateInfo skyboxInfo = PipelineConfig::CreateSkyboxPipeline(UDevice->GetSamples(), pso->pipelineLayout, pso->pipelineCache);
        pso->pPipeline = PipelineFactory::CreatePipeline(skyboxInfo, UDevice, USwapChain);
    }

    void MaterialSkybox::ReCreate()
    {
        MaterialBase::ReCreate();

        auto images = USwapChain->GetImages().size();
        CreateDescriptorSetLayout();
        CreateDescriptorPool(images);
        CreateDescriptorSets(images);
        CreatePipelineCache();
        CreatePipelineLayout(images);

        auto pso = m_mPSO.at(GetShaderSet());
        FPipelineCreateInfo skyboxInfo = PipelineConfig::CreateSkyboxPipeline(UDevice->GetSamples(), pso->pipelineLayout, pso->pipelineCache);
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

    void MaterialSkybox::CreateDescriptorSetLayout()
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

        std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, diffuseLayoutBinding};
        vk::DescriptorSetLayoutCreateInfo createInfo{};
        createInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
        createInfo.pBindings = bindings.data();

        //TODO: check result
        auto result = UDevice->GetLogical()->createDescriptorSetLayout(&createInfo, nullptr, &pso->descriptorSetLayout);
    }

    void MaterialSkybox::CreateDescriptorPool(uint32_t images)
    {
        MaterialBase::CreateDescriptorPool(images);

        auto pso = m_mPSO.at(GetShaderSet());
        std::array<vk::DescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = images;
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = 1;

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = images;

        //TODO:: add result checking
        pso->descriptorPool = UDevice->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(poolInfo);
    }

    void MaterialSkybox::CreateDescriptorSets(uint32_t images)
    {
        MaterialBase::CreateDescriptorSets(images);
    }

    void MaterialSkybox::CreatePipelineCache()
    {
        MaterialBase::CreatePipelineCache();
    }

    void MaterialSkybox::CreatePipelineLayout(uint32_t images)
    {
        MaterialBase::CreatePipelineLayout(images);
    }
}