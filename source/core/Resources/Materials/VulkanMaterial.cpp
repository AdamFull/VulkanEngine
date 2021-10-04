#include "VulkanMaterial.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/Pipeline/VulkanPipeline.h"
#include "Renderer/Pipeline/PipelineFactory.h"
#include "Resources/Textures/VulkanTexture.h"

namespace Engine
{
    void MaterialBase::Create(std::shared_ptr<Texture2D> color, 
                                 std::shared_ptr<Texture2D> normal,
                                 std::shared_ptr<Texture2D> specular)
    {
        m_pColor = color;
        m_pNormal = normal;
        m_pSpecular = specular;
    }

    void MaterialBase::ReCreate()
    {
        ResourceBase::ReCreate();
    }

    void MaterialBase::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        ResourceBase::Update(imageIndex, pUniformBuffer);
    }

    void MaterialBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        ResourceBase::Bind(commandBuffer, imageIndex);
        commandBuffer.bindDescriptorSets(m_pPipeline->GetBindPoint(), pipelineLayout, 0, 1, &vDescriptorSets[imageIndex], 0, nullptr);
        m_pPipeline->Bind(commandBuffer);
    }

    void MaterialBase::Destroy()
    {
        ResourceBase::Destroy();
        if(m_pColor)
            m_pColor->Destroy();
        if(m_pNormal)
            m_pNormal->Destroy();
        if(m_pSpecular)
            m_pSpecular->Destroy();
        if(m_pAmbient)
            m_pAmbient->Destroy();
        Cleanup();
    }

    void MaterialBase::Cleanup()
    {
        ResourceBase::Cleanup();
        m_pPipeline->Destroy(UDevice);
        UDevice->GetLogical()->freeDescriptorSets(descriptorPool, vDescriptorSets);
        UDevice->Destroy(descriptorSetLayout);
        UDevice->Destroy(descriptorPool);
        UDevice->Destroy(pipelineCache);
        UDevice->Destroy(pipelineLayout);
    }

    void MaterialBase::CreateDescriptorSets(uint32_t images)
    {
        std::vector<vk::DescriptorSetLayout> vDescriptorSetLayouts(images, descriptorSetLayout);

        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = images;
        allocInfo.pSetLayouts = vDescriptorSetLayouts.data();

        vDescriptorSets.resize(images);

        auto result = UDevice->GetLogical()->allocateDescriptorSets(&allocInfo, vDescriptorSets.data());
    }

    void MaterialBase::CreatePipelineCache()
    {
        vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCache = UDevice->GetLogical()->createPipelineCache(pipelineCacheCreateInfo);
    }

    void MaterialBase::CreatePipelineLayout(uint32_t images)
    {
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        pipelineLayout = UDevice->Make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutInfo);
        assert(pipelineLayout && "Pipeline layout was not created");
    }
}