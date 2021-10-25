#include "VulkanMaterial.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanHighLevel.h"

#include "Renderer/Descriptor/DescriptorPool.h"
#include "Renderer/Descriptor/DescriptorWriter.h"

namespace Engine
{
    FPipelineCreateInfo MaterialBase::CreateInfo(EShaderSet eSet)
    {
        switch (eSet)
        {
            case EShaderSet::eUI: return PipelineConfig::CreateUIPipeline(UDevice->GetSamples(), pipelineLayout, pipelineCache); break;
            case EShaderSet::eDiffuse: return PipelineConfig::CreateDiffusePipeline(UDevice->GetSamples(), pipelineLayout, pipelineCache); break;
            case EShaderSet::eSkybox: return PipelineConfig::CreateSkyboxPipeline(UDevice->GetSamples(), pipelineLayout, pipelineCache); break;
            case EShaderSet::ePBR: return PipelineConfig::CreatePBRPipeline(UDevice->GetSamples(), pipelineLayout, pipelineCache); break;
        }

        return FPipelineCreateInfo{};
    }

    std::map<int, ETextureAttachmentType> MaterialBase::m_mTextureBindings
    {
        {0, ETextureAttachmentType::eColor},
        {1, ETextureAttachmentType::eNormal},
        {2, ETextureAttachmentType::eSpecular}
    };

    void MaterialBase::Create(std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        uint32_t images = USwapChain->GetImages().size();
        CreateDescriptorPool(images);
        CreateDescriptors(images, pUniformBuffer);
        CreatePipelineLayout(images);
        CreatePipelineCache();

        pPipeline = PipelineFactory::CreatePipeline(CreateInfo(GetShaderSet()), UDevice, USwapChain);
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
        pPipeline->RecreatePipeline(CreateInfo(GetShaderSet()), UDevice, USwapChain);
    }

    void MaterialBase::Update(uint32_t imageIndex)
    {
        ResourceBase::Update(imageIndex);
    }

    void MaterialBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        ResourceBase::Bind(commandBuffer, imageIndex);
        commandBuffer.bindDescriptorSets(pPipeline->GetBindPoint(), pipelineLayout, 0, 1, &m_pMatWriter->GetSet(imageIndex), 0, nullptr);
        //commandBuffer.bindDescriptorSets(pPipeline->GetBindPoint(), pipelineLayout, 1, 1, &skinsSet[imageIndex], 0, nullptr);
        commandBuffer.bindDescriptorSets(pPipeline->GetBindPoint(), pipelineLayout, 2, 1, &m_pTexWriter->GetSet(imageIndex), 0, nullptr);
        pPipeline->Bind(commandBuffer);
    }

    void MaterialBase::Destroy()
    {
        ResourceBase::Destroy();
        Cleanup();
    }

    void MaterialBase::Cleanup()
    {
        ResourceBase::Cleanup();
        m_pMatWriter->Destroy(UDevice);
        m_pTexWriter->Destroy(UDevice);
        m_pDescriptorPool->Destroy(UDevice);
        UDevice->Destroy(pipelineLayout);
        UDevice->Destroy(pipelineCache);
    }

    void MaterialBase::CreateDescriptorPool(uint32_t images)
    {
        m_pDescriptorPool = VulkanDescriptorPool::Builder().
        setMaxSets(1000).
        addPoolSize(vk::DescriptorType::eUniformBuffer, 1000).
        addPoolSize(vk::DescriptorType::eCombinedImageSampler, 1000).
        build(UDevice);
    }

    void MaterialBase::CreatePipelineLayout(uint32_t images)
    {
        std::vector<vk::DescriptorSetLayout> vLayouts = {m_pMatWriter->GetSetLayout()/*, descriptors.skinsSetLayout*/, m_pTexWriter->GetSetLayout()};
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.setLayoutCount = vLayouts.size();
        pipelineLayoutInfo.pSetLayouts = vLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        pipelineLayout = UDevice->Make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutInfo);
        assert(pipelineLayout && "Pipeline layout was not created");
    }

    void MaterialBase::CreatePipelineCache()
    {
        vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCache = UDevice->GetLogical()->createPipelineCache(pipelineCacheCreateInfo);
    }
}