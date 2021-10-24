#include "VulkanMaterial.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanHighLevel.h"

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

    void MaterialBase::Create()
    {
        uint32_t images = USwapChain->GetImages().size();
        CreateDescriptorSetLayout();
        CreateDescriptorPool(images);
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

    void MaterialBase::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        ResourceBase::Update(imageIndex, pUniformBuffer);
    }

    void MaterialBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        ResourceBase::Bind(commandBuffer, imageIndex);
        commandBuffer.bindDescriptorSets(pPipeline->GetBindPoint(), pipelineLayout, 0, 1, &descriptors.matricesSet[imageIndex], 0, nullptr);
        //commandBuffer.bindDescriptorSets(pPipeline->GetBindPoint(), pipelineLayout, 1, 1, &skinsSet[imageIndex], 0, nullptr);
        commandBuffer.bindDescriptorSets(pPipeline->GetBindPoint(), pipelineLayout, 2, 1, &descriptors.texturesSet[imageIndex], 0, nullptr);
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
        UDevice->GetLogical()->freeDescriptorSets(descriptorPool, descriptors.matricesSet);
        UDevice->GetLogical()->freeDescriptorSets(descriptorPool, descriptors.skinsSet);
        UDevice->GetLogical()->freeDescriptorSets(descriptorPool, descriptors.texturesSet);
        UDevice->Destroy(descriptors.matricesSet);
        UDevice->Destroy(descriptors.skinsSet);
        UDevice->Destroy(descriptors.texturesSet);
        UDevice->Destroy(descriptorPool);
        UDevice->Destroy(pipelineLayout);
        UDevice->Destroy(pipelineCache);
    }

    void MaterialBase::CreateDescriptorSetLayout()
    {

    }

    void MaterialBase::CreateDescriptorSets(uint32_t images)
    {
        std::vector<vk::DescriptorSetLayout> vMatricesSetLayouts(images, descriptors.matricesSetLayout);
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = images;
        allocInfo.pSetLayouts = vMatricesSetLayouts.data();
        descriptors.matricesSet.resize(images);
        auto mresult = UDevice->GetLogical()->allocateDescriptorSets(&allocInfo, descriptors.matricesSet.data());

        std::vector<vk::DescriptorSetLayout> vSkinsSetLayouts(images, descriptors.skinsSetLayout);
        vk::DescriptorSetAllocateInfo skinsAllocInfo{};
        skinsAllocInfo.descriptorPool = descriptorPool;
        skinsAllocInfo.descriptorSetCount = images;
        skinsAllocInfo.pSetLayouts = vSkinsSetLayouts.data();
        descriptors.skinsSet.resize(images);
        auto sresult = UDevice->GetLogical()->allocateDescriptorSets(&allocInfo, descriptors.skinsSet.data());

        std::vector<vk::DescriptorSetLayout> vTexturesSetLayouts(images, descriptors.texturesSetLayout);
        vk::DescriptorSetAllocateInfo texturesAllocInfo{};
        texturesAllocInfo.descriptorPool = descriptorPool;
        texturesAllocInfo.descriptorSetCount = images;
        texturesAllocInfo.pSetLayouts = vTexturesSetLayouts.data();
        descriptors.texturesSet.resize(images);
        auto tresult = UDevice->GetLogical()->allocateDescriptorSets(&allocInfo, descriptors.texturesSet.data());
    }

    void MaterialBase::CreateDescriptorPool(uint32_t images)
    {
        std::array<vk::DescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = 1000;
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = 1000;

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 1000;

        descriptorPool = UDevice->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(poolInfo);
    }

    void MaterialBase::CreatePipelineLayout(uint32_t images)
    {
        std::vector<vk::DescriptorSetLayout> vLayouts = {descriptors.matricesSetLayout/*, descriptors.skinsSetLayout*/, descriptors.texturesSetLayout};
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