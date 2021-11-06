#include "VulkanMaterial.h"
#include "Core/VulkanUniform.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

FPipelineCreateInfo MaterialBase::CreateInfo(EShaderSet eSet)
{
    switch (eSet)
    {
    case EShaderSet::eUI:
        return PipelineConfig::CreateUIPipeline(renderPass, UDevice->GetSamples(), pipelineLayout, pipelineCache);
        break;
    case EShaderSet::eDiffuse:
        return PipelineConfig::CreateDiffusePipeline(renderPass, UDevice->GetSamples(), pipelineLayout, pipelineCache);
        break;
    case EShaderSet::eSkybox:
        return PipelineConfig::CreateSkyboxPipeline(renderPass, UDevice->GetSamples(), pipelineLayout, pipelineCache);
        break;
    }

    return FPipelineCreateInfo{};
}

void MaterialBase::Create()
{
    m_pMatDesc = std::make_unique<VulkanDescriptorSetContainer>();

    uint32_t images = USwapChain->GetImages().size();
    CreateDescriptorPool(images);
    CreateDescriptors(images);
    CreatePipelineLayout(images);
    CreatePipelineCache();

    pPipeline = PipelineFactory::CreatePipeline(CreateInfo(GetShaderSet()), UDevice, USwapChain);
    m_pMatDesc->UpdatePipelineInfo(pPipeline->GetBindPoint(), pipelineLayout);
}

void MaterialBase::AddTexture(ETextureAttachmentType eAttachment, std::shared_ptr<TextureBase> pTexture)
{
    m_mTextures.emplace(eAttachment, pTexture);
}

std::shared_ptr<TextureBase> MaterialBase::GetTexture(ETextureAttachmentType eAttachment)
{
    return m_mTextures[eAttachment];
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

    m_pMatDesc->Bind(commandBuffer, imageIndex);
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
    // m_pMatWriter->Destroy(UDevice);
    // m_pTexWriter->Destroy(UDevice);
    m_pDescriptorPool->Destroy(UDevice);
    UDevice->Destroy(pipelineLayout);
    UDevice->Destroy(pipelineCache);
}

void MaterialBase::CreateDescriptorPool(uint32_t images)
{
    m_pDescriptorPool = VulkanDescriptorPool::Builder().setMaxSets(1000).addPoolSize(vk::DescriptorType::eUniformBuffer, 1000).addPoolSize(vk::DescriptorType::eCombinedImageSampler, 1000).build(UDevice);
}

void MaterialBase::CreateDescriptors(uint32_t images)
{
}

void MaterialBase::CreatePipelineLayout(uint32_t images)
{
    std::vector<vk::DescriptorSetLayout> vLayouts = m_pMatDesc->GetLayouts();
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.setLayoutCount = vLayouts.size();
    pipelineLayoutInfo.pSetLayouts = vLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = m_vConstantRanges.size();
    pipelineLayoutInfo.pPushConstantRanges = m_vConstantRanges.data();

    pipelineLayout = UDevice->Make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutInfo);
    assert(pipelineLayout && "Pipeline layout was not created");
}

void MaterialBase::CreatePipelineCache()
{
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCache = UDevice->GetLogical()->createPipelineCache(pipelineCacheCreateInfo);
}