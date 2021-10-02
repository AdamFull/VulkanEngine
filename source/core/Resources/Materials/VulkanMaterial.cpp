#include "VulkanMaterial.h"
#include "Resources/Textures/VulkanTexture.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/Pipeline/VulkanPipeline.h"
#include "Renderer/Pipeline/PipelineFactory.h"

namespace Engine
{
    void MaterialBase::Create(std::shared_ptr<Texture2D> color)
    {
        m_pColor = color;

        CreateDescriptorSetLayout();
        CreateDescriptorPool();
        CreateDescriptorSets();
        CreatePipelineLayout();

        FPipelineCreateInfo createInfo = PipelineConfig::CreateDefaultPipelineConfig(UDevice->GetSamples(), m_pipelineLayout);
        m_pPipeline = PipelineFactory::CreatePipeline(createInfo, UDevice, USwapChain, EPipelineType::eGraphics);
    }

    void MaterialBase::ReCreate()
    {
        ResourceBase::ReCreate();
        CreateDescriptorSetLayout();
        CreateDescriptorPool();
        CreateDescriptorSets();
        CreatePipelineLayout();

        FPipelineCreateInfo createInfo = PipelineConfig::CreateDefaultPipelineConfig(UDevice->GetSamples(), m_pipelineLayout);
        m_pPipeline->RecreatePipeline(createInfo, UDevice, USwapChain);
    }

    void MaterialBase::Update(uint32_t imageIndex)
    {
        ResourceBase::Update(imageIndex);
    }

    void MaterialBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        ResourceBase::Bind(commandBuffer, imageIndex);
        m_pPipeline->Bind(commandBuffer);
        UUniform->Bind(commandBuffer, m_pipelineLayout, m_vDescriptorSets[imageIndex]);
    }

    void MaterialBase::Destroy()
    {
        ResourceBase::Destroy();
        m_pColor->Destroy();
        Cleanup();
    }

    void MaterialBase::Cleanup()
    {
        ResourceBase::Cleanup();
        
        m_pPipeline->Destroy(UDevice);
        UDevice->Destroy(m_pipelineLayout);
        UDevice->GetLogical()->freeDescriptorSets(m_descriptorPool, m_vDescriptorSets);
        UDevice->Destroy(m_descriptorSetLayout);
        UDevice->Destroy(m_descriptorPool);
    }

    void MaterialBase::CreateDescriptorSets()
    {
        uint32_t images = USwapChain->GetImages().size();
        std::vector<vk::DescriptorSetLayout> vDescriptorSetLayouts(images, m_descriptorSetLayout);

        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = images;
        allocInfo.pSetLayouts = vDescriptorSetLayouts.data();

        m_vDescriptorSets.resize(images);

        auto result = UDevice->GetLogical()->allocateDescriptorSets(&allocInfo, m_vDescriptorSets.data());
    }

    /****************************************************DiffuseMaterial***************************************************************/
    void MaterialDiffuse::Create(std::shared_ptr<Texture2D> color)
    {
        MaterialBase::Create(color);
    }

    void MaterialDiffuse::ReCreate()
    {
        MaterialBase::ReCreate();
    }

    void MaterialDiffuse::Update(uint32_t imageIndex)
    {
        MaterialBase::Update(imageIndex);

        auto& uniformBuffer = UUniform->GetUniformBuffer(imageIndex)->GetBuffer();
        auto& descriptorSet = m_vDescriptorSets[imageIndex];

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
        descriptorWrites[1].pImageInfo = &m_pColor->GetDescriptor();

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
        /*
        m_pAmbient->Destroy();
        m_pSpecular->Destroy();
        m_pNormal->Destroy();
        */
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

        vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
        vk::DescriptorSetLayoutCreateInfo createInfo{};
        createInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
        createInfo.pBindings = bindings.data();

        //TODO: check result
        auto result = UDevice->GetLogical()->createDescriptorSetLayout(&createInfo, nullptr, &m_descriptorSetLayout);
    }

    void MaterialDiffuse::CreateDescriptorPool()
    {
        MaterialBase::CreateDescriptorPool();

        uint32_t images = USwapChain->GetImages().size();

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
        m_descriptorPool = UDevice->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(poolInfo);
    }

    void MaterialDiffuse::CreateDescriptorSets()
    {
        MaterialBase::CreateDescriptorSets();
    }

    void MaterialDiffuse::CreatePipelineLayout()
    {
        MaterialBase::CreatePipelineLayout();

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        m_pipelineLayout = UDevice->Make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutInfo);
        assert(m_pipelineLayout && "Pipeline layout was not created");
    }
}