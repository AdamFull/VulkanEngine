#include "VulkanMaterial.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"

using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

MaterialBase::~MaterialBase()
{
}

void MaterialBase::Create(vk::RenderPass& renderPass, uint32_t subpass)
{
    uint32_t images = USwapChain->GetImages().size();
    m_pDescriptorSet = std::make_unique<DescriptorHandler>();
    m_pDescriptorSet->Create(m_pPipeline);
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
    m_pPipeline->RecreatePipeline();
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