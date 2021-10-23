#include "VulkanMaterial.h"
#include "Renderer/VulkanUniform.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/Pipeline/PipelineManager.h"

namespace Engine
{
    std::map<int, ETextureAttachmentType> MaterialBase::m_mTextureBindings
    {
        {1, ETextureAttachmentType::eColor},
        {2, ETextureAttachmentType::eNormal},
        {3, ETextureAttachmentType::eSpecular}
    };

    void MaterialBase::Create()
    {
        pPipeline = UPipelineMGR->Get(GetShaderSet());
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
        ResourceBase::Bind(commandBuffer, imageIndex);
        commandBuffer.bindDescriptorSets(pPipeline->GetBindPoint(), UPMGR_PL, 0, 1, &vDescriptorSets[imageIndex], 0, nullptr);
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
        UDevice->GetLogical()->freeDescriptorSets(UPMGR_DP, vDescriptorSets);
    }

    void MaterialBase::CreateDescriptorSets(uint32_t images)
    {
        std::vector<vk::DescriptorSetLayout> vDescriptorSetLayouts(images, UPMGR_DSL);

        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = UPMGR_DP;
        allocInfo.descriptorSetCount = images;
        allocInfo.pSetLayouts = vDescriptorSetLayouts.data();

        vDescriptorSets.resize(images);

        auto result = UDevice->GetLogical()->allocateDescriptorSets(&allocInfo, vDescriptorSets.data());
    }
}