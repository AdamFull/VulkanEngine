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
    if(!bIsCreated)
    {
        uint32_t images = CDevice::inst()->getFramesInFlight();
        auto& renderPass = CRenderSystem::inst()->getCurrentStage()->getCurrentFramebuffer()->getRenderPass();
        auto subpass = CRenderSystem::inst()->getCurrentStage()->getCurrentFramebuffer()->getCurrentSubpass();

        pPipeline->create(renderPass, subpass);

        for(auto instance = 0; instance < instances; instance++)
        {
            auto instance_ptr = make_scope<FMaterialUniqueObjects>();
            instance_ptr->pDescriptorSet = make_scope<CDescriptorHandler>();
            instance_ptr->pDescriptorSet->create(pPipeline);

            for(auto& [name, uniform] : pPipeline->getShader()->getUniformBlocks())
            {
                ref_ptr<CHandler> pUniform;
                switch(uniform.getDescriptorType())
                {
                    case vk::DescriptorType::eUniformBuffer: pUniform = make_ref<CUniformHandler>(); break;
                    case vk::DescriptorType::eStorageBuffer: pUniform = make_ref<CStorageHandler>(); break;
                }
                pUniform->create(uniform);
                instance_ptr->mBuffers.emplace(name, pUniform);
            }

            for(auto& [name, uniform] : pPipeline->getShader()->getPushBlocks())
            {
                auto pUniform = make_ref<CPushHandler>();
                pUniform->create(uniform, pPipeline);
                instance_ptr->mPushConstants.emplace(name, pUniform);
            }

            vInstances.emplace_back(std::move(instance_ptr));
        }
        bIsCreated = true;
    }
}

void CMaterialBase::addTexture(const std::string& attachment, vk::DescriptorImageInfo& descriptor)
{
    mTextures[attachment] = descriptor;
}

void CMaterialBase::addTexture(const std::string& attachment, ref_ptr<CImage>& pTexture)
{
    mTextures[attachment] = pTexture->getDescriptor();
}

vk::DescriptorImageInfo& CMaterialBase::getTexture(const std::string& attachment)
{
    return mTextures[attachment];
}

void CMaterialBase::reCreate()
{
    if(!bIsReCreated)
    {
        auto& renderPass = CRenderSystem::inst()->getCurrentStage()->getCurrentFramebuffer()->getRenderPass();
        auto subpass = CRenderSystem::inst()->getCurrentStage()->getCurrentFramebuffer()->getCurrentSubpass();
        pPipeline->reCreate(renderPass, subpass);
        bIsReCreated = true;
    }
}

void CMaterialBase::update()
{
    auto& pDescriptorSet = getDescriptorSet();
    auto& mBuffers = getUniformBuffers();
    pDescriptorSet->reset();
    for(auto& [name, uniform] : mBuffers)
    {
        uniform->flush();
        auto& buffer = uniform->getBuffer();
        auto descriptor = buffer->getDescriptor();
        pDescriptorSet->set(name, descriptor);
    }

    for(auto& [key, texture] : mTextures)
        pDescriptorSet->set(key, texture);
    pDescriptorSet->update();
}

void CMaterialBase::bind()
{
    auto& commandBuffer = CRenderSystem::inst()->getCurrentCommandBuffer();
    auto& pDescriptorSet = getDescriptorSet();
    pDescriptorSet->bind(commandBuffer);
    pPipeline->bind(commandBuffer);
    bIsReCreated = false;
    currentInstance = (currentInstance + 1) % instances;
}

void CMaterialBase::cleanup()
{
    //Custom cleanup rules
    for(auto& instance : vInstances)
    {
        if(instance->pDescriptorSet)
            instance->pDescriptorSet->cleanup();
        for(auto& [name, handler] : instance->mBuffers)
            handler->cleanup();
        instance->mBuffers.clear();
        for(auto& [name, handler] : instance->mPushConstants)
            handler->cleanup();
        instance->mPushConstants.clear();
    }
    pPipeline->cleanup();
}

void CMaterialBase::setName(const std::string& srName)
{
    m_srName = srName + uuid::generate();
}

void CMaterialBase::setInstances(uint32_t instances)
{
    this->instances = instances;
}

void CMaterialBase::incrementUsageCount()
{
    instances++;
}

void CMaterialBase::addDefinition(const std::string& definition, const std::string& value)
{
    pPipeline->addDefine(definition, value);
}