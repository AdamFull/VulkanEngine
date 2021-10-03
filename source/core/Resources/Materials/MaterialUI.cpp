#include "MaterialUI.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/Pipeline/VulkanPipeline.h"
#include "Renderer/Pipeline/PipelineFactory.h"
#include "Resources/Textures/VulkanTexture.h"

namespace Engine
{
    void MaterialUI::Create(std::shared_ptr<Texture2D> color, 
                                 std::shared_ptr<Texture2D> normal,
                                 std::shared_ptr<Texture2D> specular)
    {
        MaterialBase::Create(color, normal, specular);

        CreateDescriptorSetLayout();
        CreateDescriptorPool(USwapChain->GetImages().size());
        CreateDescriptorSets(USwapChain->GetImages().size());
        CreatePipelineLayout(USwapChain->GetImages().size());

        FPipelineCreateInfo createInfo = PipelineConfig::CreateDefaultPipelineConfig(EPipelineType::eGraphics, EShaderSet::eUI, UDevice->GetSamples(), pipelineLayout, pipelineCache);
        m_pPipeline = PipelineFactory::CreatePipeline(createInfo, UDevice, USwapChain);
    }

    void MaterialUI::ReCreate()
    {
        MaterialBase::ReCreate();

        CreateDescriptorSetLayout();
        CreateDescriptorPool(USwapChain->GetImages().size());
        CreateDescriptorSets(USwapChain->GetImages().size());
        CreatePipelineCache();
        CreatePipelineLayout(USwapChain->GetImages().size());

        FPipelineCreateInfo createInfo = PipelineConfig::CreateDefaultPipelineConfig(EPipelineType::eGraphics, EShaderSet::eUI, UDevice->GetSamples(), pipelineLayout, pipelineCache);
        m_pPipeline->RecreatePipeline(createInfo, UDevice, USwapChain);
    }

    void MaterialUI::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        MaterialBase::Update(imageIndex, pUniformBuffer);

        auto& uniformBuffer = pUniformBuffer->GetBuffer();
        auto& descriptorSet = vDescriptorSets[imageIndex];

        vk::DescriptorBufferInfo bufferInfo{};
        //GetCurrentUniform
        bufferInfo.buffer = uniformBuffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(FUniformDataUI);

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
        descriptorWrites[1].pImageInfo = &m_pColor->GetDescriptor();

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

    void MaterialUI::CreateDescriptorSetLayout()
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

        std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, diffuseLayoutBinding};
        vk::DescriptorSetLayoutCreateInfo createInfo{};
        createInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
        createInfo.pBindings = bindings.data();

        //TODO: check result
        auto result = UDevice->GetLogical()->createDescriptorSetLayout(&createInfo, nullptr, &descriptorSetLayout);
    }

    void MaterialUI::CreateDescriptorPool(uint32_t images)
    {
        MaterialBase::CreateDescriptorPool(images);

        std::array<vk::DescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = images;
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = images;

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = images;

        //TODO:: add result checking
        descriptorPool = UDevice->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(poolInfo);
    }

    void MaterialUI::CreateDescriptorSets(uint32_t images)
    {
        MaterialBase::CreateDescriptorSets(images);
    }

    void MaterialUI::CreatePipelineCache()
    {
        MaterialBase::CreatePipelineCache();
    }

    void MaterialUI::CreatePipelineLayout(uint32_t images)
    {
        MaterialBase::CreatePipelineLayout(images);
    }
}