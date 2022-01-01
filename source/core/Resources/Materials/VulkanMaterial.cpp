#include "VulkanMaterial.h"
#include "Core/VulkanHighLevel.h"
#include "Resources/ResourceManager.h"

using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

MaterialBase::~MaterialBase()
{
}

void MaterialBase::Create(std::shared_ptr<ResourceManager> pResMgr, vk::RenderPass& rPass)
{
    renderPass = rPass;
    MaterialBase::Create(pResMgr);
}

void MaterialBase::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    uint32_t images = USwapChain->GetImages().size();

    initial.renderPass = renderPass;
    m_pPipeline = PipelineFactory::CreatePipeline(PipelineConfig::MakeInfo(initial));
    m_pDescriptorSet = std::make_unique<DescriptorHandler>();
    m_pDescriptorSet->Create(m_pPipeline);
    //m_pMatDesc->UpdatePipelineInfo(pPipeline->GetBindPoint(), pipelineLayout);
}

void MaterialBase::AddTexture(ETextureAttachmentType eAttachment, vk::DescriptorImageInfo& descriptor)
{
    m_mTextures[eAttachment] = descriptor;
}

void MaterialBase::AddTexture(ETextureAttachmentType eAttachment, std::shared_ptr<Core::Image> pTexture)
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
    m_pPipeline->RecreatePipeline(PipelineConfig::MakeInfo(initial));
}

void MaterialBase::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->Update(imageIndex);
}

void MaterialBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    m_pDescriptorSet->Bind(commandBuffer, imageIndex);
    m_pPipeline->Bind(commandBuffer);
}

void MaterialBase::Cleanup()
{

}

void MaterialBase::SetName(const std::string& srName)
{
    m_srName = srName + uuid::generate();
}