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

void MaterialBase::AddTexture(const std::string& attachment, vk::DescriptorImageInfo& descriptor)
{
    m_mTextures[attachment] = descriptor;
}

void MaterialBase::AddTexture(const std::string& attachment, std::shared_ptr<Core::Image> pTexture)
{
    m_mTextures[attachment] = pTexture->GetDescriptor();
}

vk::DescriptorImageInfo& MaterialBase::GetTexture(const std::string& attachment)
{
    return m_mTextures[attachment];
}

void MaterialBase::ReCreate()
{
    m_pPipeline->recreatePipeline();
}

void MaterialBase::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    m_pDescriptorSet->Update(imageIndex);
}

void MaterialBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    m_pDescriptorSet->Bind(commandBuffer, imageIndex);
    m_pPipeline->bind(commandBuffer);
}

void MaterialBase::Cleanup()
{

}

void MaterialBase::SetName(const std::string& srName)
{
    m_srName = srName + uuid::generate();
}