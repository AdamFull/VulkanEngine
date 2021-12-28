#include "VulkanMaterial.h"
#include "Core/VulkanHighLevel.h"
#include "Resources/ResourceManager.h"

using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

MaterialBase::~MaterialBase()
{
    UDevice->Destroy(pipelineLayout);
    UDevice->Destroy(pipelineCache);
}

/*FPipelineCreateInfo MaterialBase::CreateInfo(EShaderSet eSet)
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
    case EShaderSet::ePostProcess:
        return PipelineConfig::CreatePostProcessPipeline(renderPass, pipelineLayout, pipelineCache);
        break;
    }

    return FPipelineCreateInfo{};
}*/

void MaterialBase::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    m_pMatDesc = std::make_unique<VulkanDescriptorSetContainer>();

    uint32_t images = USwapChain->GetImages().size();
    CreateDescriptorPool(images);
    CreateDescriptors(images);
    CreatePipelineLayout(images);
    CreatePipelineCache();

    initial.renderPass = renderPass;
    initial.pipelineLayout = pipelineLayout;
    initial.pipelineCache = pipelineCache;
    pPipeline = PipelineFactory::CreatePipeline(PipelineConfig::MakeInfo(initial));
    m_pMatDesc->UpdatePipelineInfo(pPipeline->GetBindPoint(), pipelineLayout);
}

void MaterialBase::AddTexture(ETextureAttachmentType eAttachment, vk::DescriptorImageInfo& descriptor)
{
    m_mTextures[eAttachment] = descriptor;
}

void MaterialBase::AddTexture(ETextureAttachmentType eAttachment, std::shared_ptr<Texture::Image> pTexture)
{
    m_mTextures[eAttachment] = pTexture->GetDescriptor();
}

vk::DescriptorImageInfo& MaterialBase::GetTexture(ETextureAttachmentType eAttachment)
{
    return m_mTextures[eAttachment];
}

void MaterialBase::ReCreate()
{
    initial.renderPass = renderPass;
    initial.pipelineLayout = pipelineLayout;
    initial.pipelineCache = pipelineCache;
    pPipeline->RecreatePipeline(PipelineConfig::MakeInfo(initial));
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

    pipelineLayout = UDevice->Make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutInfo);
    assert(pipelineLayout && "Pipeline layout was not created");
}

void MaterialBase::CreatePipelineCache()
{
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCache = UDevice->GetLogical().createPipelineCache(pipelineCacheCreateInfo);
}

void MaterialBase::SetName(const std::string& srName)
{
    m_srName = srName + uuid::generate();
}