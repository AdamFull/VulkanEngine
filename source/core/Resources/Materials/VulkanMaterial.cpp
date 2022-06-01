#include "VulkanMaterial.h"
#include "util/uuid.hpp"
#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"
#include "graphics/buffer/UniformHandler.hpp"
#include "graphics/buffer/StorageHandler.h"

using namespace engine::core;
using namespace engine::core::render;
using namespace engine::resources::material;
using namespace engine::core::descriptor;
using namespace engine::core::pipeline;

CMaterialBase::~CMaterialBase()
{
    cleanup();
}

void CMaterialBase::create()
{
    //Material loads render pass and subpass number from attached render stage
    //Material should be in render stage because render stage contains specific render pass and subpass
    uint32_t images = CDevice::inst()->getFramesInFlight();
    auto& renderPass = CRenderSystem::inst()->getCurrentStage()->getCurrentFramebuffer()->getRenderPass();
    auto subpass = CRenderSystem::inst()->getCurrentStage()->getCurrentFramebuffer()->getCurrentSubpass();
    m_pPipeline->create(renderPass, subpass);
    m_pDescriptorSet = make_scope<CDescriptorHandler>();
    m_pDescriptorSet->create(m_pPipeline);

    for(auto& [name, uniform] : m_pPipeline->getShader()->getUniformBlocks())
    {
        ref_ptr<CHandler> pUniform;
        switch(uniform.getDescriptorType())
        {
            case vk::DescriptorType::eUniformBuffer: pUniform = make_ref<CUniformHandler>(); break;
            case vk::DescriptorType::eStorageBuffer: pUniform = make_ref<CStorageHandler>(); break;
        }
        pUniform->create(uniform);
        mBuffers.emplace(name, pUniform);
    }

    for(auto& [name, uniform] : m_pPipeline->getShader()->getPushBlocks())
    {
        auto pUniform = make_ref<CPushHandler>();
        pUniform->create(uniform, m_pPipeline);
        mPushConstants.emplace(name, pUniform);
    }
}

void CMaterialBase::addTexture(const std::string& attachment, vk::DescriptorImageInfo& descriptor)
{
    m_mTextures[attachment] = descriptor;
}

void CMaterialBase::addTexture(const std::string& attachment, ref_ptr<CImage>& pTexture)
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
    auto& renderPass = CRenderSystem::inst()->getCurrentStage()->getCurrentFramebuffer()->getRenderPass();
    auto subpass = CRenderSystem::inst()->getCurrentStage()->getCurrentFramebuffer()->getCurrentSubpass();
    m_pPipeline->reCreate(renderPass, subpass);
}

void CMaterialBase::update()
{
    m_pDescriptorSet->reset();
    for(auto& [name, uniform] : mBuffers)
    {
        uniform->flush();
        auto& buffer = uniform->getBuffer();
        auto descriptor = buffer->getDescriptor();
        m_pDescriptorSet->set(name, descriptor);
    }

    for(auto& [key, texture] : m_mTextures)
        m_pDescriptorSet->set(key, texture);
    m_pDescriptorSet->update();
}

void CMaterialBase::bind()
{
    auto& commandBuffer = CRenderSystem::inst()->getCurrentCommandBuffer();
    m_pDescriptorSet->bind(commandBuffer);
    m_pPipeline->bind(commandBuffer);
}

void CMaterialBase::cleanup()
{
    //Custom cleanup rules
    if(m_pDescriptorSet)
        m_pDescriptorSet->cleanup();
    for(auto& [name, handler] : mBuffers)
        handler->cleanup();
    mBuffers.clear();
    for(auto& [name, handler] : mPushConstants)
        handler->cleanup();
    mPushConstants.clear();
    m_pPipeline->cleanup();
}

void CMaterialBase::setName(const std::string& srName)
{
    m_srName = srName + uuid::generate();
}