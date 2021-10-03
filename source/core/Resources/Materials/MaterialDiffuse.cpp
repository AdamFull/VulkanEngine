#include "MaterialDiffuse.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/Pipeline/VulkanPipeline.h"
#include "Renderer/Pipeline/PipelineFactory.h"
#include "Resources/Textures/VulkanTexture.h"

namespace Engine
{
    void MaterialDiffuse::Create(std::shared_ptr<Texture2D> color, 
                                 std::shared_ptr<Texture2D> normal,
                                 std::shared_ptr<Texture2D> specular)
    {
        MaterialBase::Create(color, normal, specular);

        auto images = USwapChain->GetImages().size();
        CreateDescriptorSetLayout();
        CreateDescriptorPool(images);
        CreateDescriptorSets(images);
        CreatePipelineLayout(images);

        FPipelineCreateInfo createInfo = PipelineConfig::CreateDefaultPipelineConfig(EPipelineType::eGraphics, EShaderSet::eDiffuse, UDevice->GetSamples(), pipelineLayout, pipelineCache);
        m_pPipeline = PipelineFactory::CreatePipeline(createInfo, UDevice, USwapChain);
    }

    void MaterialDiffuse::ReCreate()
    {
        MaterialBase::ReCreate();

        auto images = USwapChain->GetImages().size();
        CreateDescriptorSetLayout();
        CreateDescriptorPool(images);
        CreateDescriptorSets(images);
        CreatePipelineCache();
        CreatePipelineLayout(images);

        FPipelineCreateInfo createInfo = PipelineConfig::CreateDefaultPipelineConfig(EPipelineType::eGraphics, EShaderSet::eDiffuse, UDevice->GetSamples(), pipelineLayout, pipelineCache);
        m_pPipeline->RecreatePipeline(createInfo, UDevice, USwapChain);
    }

    void MaterialDiffuse::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        MaterialBase::Update(imageIndex, pUniformBuffer);

        auto& uniformBuffer = UUniform->GetUniformBuffer(imageIndex)->GetBuffer();
        auto& descriptorSet = vDescriptorSets[imageIndex];

        vk::DescriptorBufferInfo bufferInfo{};
        //GetCurrentUniform
        bufferInfo.buffer = uniformBuffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(FUniformData);

        std::array<vk::WriteDescriptorSet, 4> descriptorWrites{};
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
        descriptorWrites[1].pImageInfo = &m_pColor->GetDescriptor();

        descriptorWrites[2].dstSet = descriptorSet;
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pImageInfo = &m_pNormal->GetDescriptor();

        descriptorWrites[3].dstSet = descriptorSet;
        descriptorWrites[3].dstBinding = 3;
        descriptorWrites[3].dstArrayElement = 0;
        descriptorWrites[3].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrites[3].descriptorCount = 1;
        descriptorWrites[3].pImageInfo = &m_pSpecular->GetDescriptor();

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

    void MaterialDiffuse::CreateDescriptorSetLayout()
    {
        MaterialBase::CreateDescriptorSetLayout();

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

        std::array<vk::DescriptorSetLayoutBinding, 4> bindings = {uboLayoutBinding, diffuseLayoutBinding, normalLayoutBinding, specularLayoutBinding};
        vk::DescriptorSetLayoutCreateInfo createInfo{};
        createInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
        createInfo.pBindings = bindings.data();

        //TODO: check result
        auto result = UDevice->GetLogical()->createDescriptorSetLayout(&createInfo, nullptr, &descriptorSetLayout);
    }

    void MaterialDiffuse::CreateDescriptorPool(uint32_t images)
    {
        MaterialBase::CreateDescriptorPool(images);

        std::array<vk::DescriptorPoolSize, 4> poolSizes{};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = images;
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = 1;
        poolSizes[2].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[2].descriptorCount = 1;
        poolSizes[3].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[3].descriptorCount = 1;

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = images;

        //TODO:: add result checking
        descriptorPool = UDevice->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(poolInfo);
    }

    void MaterialDiffuse::CreateDescriptorSets(uint32_t images)
    {
        MaterialBase::CreateDescriptorSets(images);
    }

    void MaterialDiffuse::CreatePipelineCache()
    {
        MaterialBase::CreatePipelineCache();
    }

    void MaterialDiffuse::CreatePipelineLayout(uint32_t images)
    {
        MaterialBase::CreatePipelineLayout(images);
    }
}