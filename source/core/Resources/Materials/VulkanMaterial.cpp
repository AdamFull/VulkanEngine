#include "VulkanMaterial.h"
#include "util/uuid.hpp"
#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"

using namespace Engine::Core;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

CMaterialBase::~CMaterialBase()
{
}

void CMaterialBase::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    uint32_t images = CSwapChain::getInstance()->getFramesInFlight();
    m_pPipeline->create(renderPass, subpass);
    m_pDescriptorSet = std::make_unique<CDescriptorHandler>();
    m_pDescriptorSet->create(m_pPipeline);
}

void CMaterialBase::addTexture(const std::string& attachment, vk::DescriptorImageInfo& descriptor)
{
    m_mTextures[attachment] = descriptor;
}

void CMaterialBase::addTexture(const std::string& attachment, std::shared_ptr<Core::CImage> pTexture)
{
    m_mTextures[attachment] = pTexture->getDescriptor();
}

void CMaterialBase::addBuffer(const std::string& attachment, vk::DescriptorBufferInfo& descriptor)
{
    m_mBuffers[attachment] = descriptor;
}

vk::DescriptorImageInfo& CMaterialBase::getTexture(const std::string& attachment)
{
    return m_mTextures[attachment];
}

void CMaterialBase::reCreate()
{
    m_pPipeline->recreatePipeline();
}

void CMaterialBase::update(uint32_t imageIndex)
{
    m_pDescriptorSet->clear();
    for(auto& [key, buffer] : m_mBuffers)
        m_pDescriptorSet->set(key, buffer);
    for(auto& [key, texture] : m_mTextures)
        m_pDescriptorSet->set(key, texture);
    m_pDescriptorSet->update(imageIndex);
}

void CMaterialBase::bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    m_pDescriptorSet->bind(commandBuffer, imageIndex);
    m_pPipeline->bind(commandBuffer);
}

void CMaterialBase::cleanup()
{

}

void CMaterialBase::setName(const std::string& srName)
{
    m_srName = srName + uuid::generate();
}