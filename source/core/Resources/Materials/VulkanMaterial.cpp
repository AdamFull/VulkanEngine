#include "VulkanMaterial.h"
#include "Core/VulkanUniform.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanSwapChain.h"
#include "Resources/ResourceManager.h"

using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

MaterialBase::MaterialBase(std::shared_ptr<Core::Device> device, std::shared_ptr<Core::SwapChain> swapchain) :
m_device(device),
m_swapchain(swapchain)
{

}

MaterialBase::~MaterialBase()
{
    m_device->Destroy(pipelineLayout);
    m_device->Destroy(pipelineCache);
}

FPipelineCreateInfo MaterialBase::CreateInfo(EShaderSet eSet)
{
    switch (eSet)
    {
    case EShaderSet::eUI:
        return PipelineConfig::CreateUIPipeline(renderPass, pipelineLayout, pipelineCache);
        break;
    case EShaderSet::eDiffuse:
        return PipelineConfig::CreateDiffusePipeline(renderPass, pipelineLayout, pipelineCache);
        break;
    case EShaderSet::eSkybox:
        return PipelineConfig::CreateSkyboxPipeline(renderPass, pipelineLayout, pipelineCache);
        break;
    case EShaderSet::eDeferred:
        return PipelineConfig::CreateDeferredPipeline(renderPass, pipelineLayout, pipelineCache);
        break;
    }

    return FPipelineCreateInfo{};
}

void MaterialBase::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    m_pMatDesc = std::make_unique<VulkanDescriptorSetContainer>();

    uint32_t images = m_swapchain->GetImages().size();
    CreateDescriptorPool(images);
    CreateDescriptors(images);
    CreatePipelineLayout(images);
    CreatePipelineCache();

    pPipeline = PipelineFactory::CreatePipeline(CreateInfo(GetShaderSet()));
    m_pMatDesc->UpdatePipelineInfo(pPipeline->GetBindPoint(), pipelineLayout);
}

void MaterialBase::AddTexture(ETextureAttachmentType eAttachment, vk::DescriptorImageInfo& descriptor)
{
    m_mTextures[eAttachment] = descriptor;
}

void MaterialBase::AddTexture(ETextureAttachmentType eAttachment, std::shared_ptr<Texture::TextureBase> pTexture)
{
    m_mTextures[eAttachment] = pTexture->GetDescriptor();
}

vk::DescriptorImageInfo& MaterialBase::GetTexture(ETextureAttachmentType eAttachment)
{
    return m_mTextures[eAttachment];
}

void MaterialBase::ReCreate()
{
    pPipeline->RecreatePipeline(CreateInfo(GetShaderSet()));
}

void MaterialBase::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{

}

void MaterialBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    m_pMatDesc->Bind(commandBuffer, imageIndex);
    pPipeline->Bind(commandBuffer);
}

void MaterialBase::Cleanup()
{

}

void MaterialBase::CreateDescriptorPool(uint32_t images)
{
    m_pDescriptorPool = VulkanDescriptorPool::Builder().
    setMaxSets(1000).
    addPoolSize(vk::DescriptorType::eUniformBuffer, 1000).
    addPoolSize(vk::DescriptorType::eCombinedImageSampler, 1000).
    setPoolFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet).
    build();
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

    pipelineLayout = m_device->Make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutInfo);
    assert(pipelineLayout && "Pipeline layout was not created");
}

void MaterialBase::CreatePipelineCache()
{
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCache = m_device->GetLogical().createPipelineCache(pipelineCacheCreateInfo);
}