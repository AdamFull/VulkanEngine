#include "VulkanMaterial.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanHighLevel.h"

namespace Engine
{
    void MaterialBase::Create()
    {
        m_mPSO.emplace(GetShaderSet(), std::make_shared<FPipelineState>());
    }

    void MaterialBase::AddTexture(ETextureAttachmentType eAttachment, std::shared_ptr<TextureBase> pTexture)
    {
        m_mTextures.emplace(eAttachment, pTexture);
    }

    void MaterialBase::AddTextures(std::map<ETextureAttachmentType, std::shared_ptr<TextureBase>> mTextures)
    {
        m_mTextures = mTextures;
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
        auto pso = m_mPSO.at(GetShaderSet());
        ResourceBase::Bind(commandBuffer, imageIndex);
        commandBuffer.bindDescriptorSets(pso->pPipeline->GetBindPoint(), pso->pipelineLayout, 0, 1, &pso->vDescriptorSets[imageIndex], 0, nullptr);
        pso->pPipeline->Bind(commandBuffer);
    }

    void MaterialBase::Destroy()
    {
        ResourceBase::Destroy();
        Cleanup();
    }

    void MaterialBase::Cleanup()
    {
        ResourceBase::Cleanup();

        for(auto& [key, pso] : m_mPSO)
        {
            pso->pPipeline->Destroy(UDevice);
            UDevice->GetLogical()->freeDescriptorSets(pso->descriptorPool, pso->vDescriptorSets);
            UDevice->Destroy(pso->descriptorSetLayout);
            UDevice->Destroy(pso->descriptorPool);
            UDevice->Destroy(pso->pipelineCache);
            UDevice->Destroy(pso->pipelineLayout);
        }
    }

    void MaterialBase::CreateDescriptorSets(uint32_t images)
    {
        auto pso = m_mPSO.at(GetShaderSet());
        std::vector<vk::DescriptorSetLayout> vDescriptorSetLayouts(images, pso->descriptorSetLayout);

        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = pso->descriptorPool;
        allocInfo.descriptorSetCount = images;
        allocInfo.pSetLayouts = vDescriptorSetLayouts.data();

        pso->vDescriptorSets.resize(images);

        auto result = UDevice->GetLogical()->allocateDescriptorSets(&allocInfo, pso->vDescriptorSets.data());
    }

    void MaterialBase::CreatePipelineCache()
    {
        auto pso = m_mPSO.at(GetShaderSet());
        vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pso->pipelineCache = UDevice->GetLogical()->createPipelineCache(pipelineCacheCreateInfo);
    }

    void MaterialBase::CreatePipelineLayout(uint32_t images)
    {
        auto pso = m_mPSO.at(GetShaderSet());
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &pso->descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        pso->pipelineLayout = UDevice->Make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutInfo);
        assert(pso->pipelineLayout && "Pipeline layout was not created");
    }
}