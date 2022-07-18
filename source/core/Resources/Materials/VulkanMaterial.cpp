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
        auto& shader = pPipeline->getShader();
        shader->setDescriptorMultiplier(images * instances);

        if(pPipeline->getBindPoint() != vk::PipelineBindPoint::eCompute)
        {
            auto& renderPass = CRenderSystem::inst()->getCurrentStage()->getCurrentFramebuffer()->getRenderPass();
            auto subpass = CRenderSystem::inst()->getCurrentStage()->getCurrentFramebuffer()->getCurrentSubpass();
            pPipeline->create(renderPass, subpass);
        }
        else
        {
            pPipeline->create();
        }
        

        for(auto instance = 0; instance < instances; instance++)
        {
            auto instance_ptr = utl::make_scope<FMaterialUniqueObjects>();
            instance_ptr->pDescriptorSet = utl::make_scope<CDescriptorHandler>(pPipeline);

            for(auto& [name, uniform] : pPipeline->getShader()->getUniformBlocks())
            {
                utl::ref_ptr<CHandler> pUniform;
                switch(uniform.getDescriptorType())
                {
                    case vk::DescriptorType::eUniformBuffer: pUniform = utl::make_ref<CUniformHandler>(uniform); break;
                    case vk::DescriptorType::eStorageBuffer: pUniform = utl::make_ref<CStorageHandler>(uniform); break;
                }
                instance_ptr->mBuffers.emplace(name, pUniform);
            }

            for(auto& [name, uniform] : pPipeline->getShader()->getPushBlocks())
            {
                auto pUniform = utl::make_ref<CPushHandler>(uniform, pPipeline);
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

void CMaterialBase::addTexture(const std::string& attachment, utl::ref_ptr<CImage>& pTexture)
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
        auto& buffer = uniform->getBuffer();
        auto& descriptor = buffer->getDescriptor();
        pDescriptorSet->set(name, descriptor);
        uniform->flush();
    }

    for(auto& [key, texture] : mTextures)
        pDescriptorSet->set(key, texture);
    pDescriptorSet->update();
}

void CMaterialBase::bind(vk::CommandBuffer& commandBuffer)
{
    auto& pDescriptorSet = getDescriptorSet();
    pDescriptorSet->bind(commandBuffer);
    pPipeline->bind(commandBuffer);
    bIsReCreated = false;
    currentInstance = (currentInstance + 1) % instances;
}

void CMaterialBase::cleanup()
{
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

utl::ref_ptr<CHandler>& CMaterialBase::getUniformBuffer(const std::string& name) 
{ 
    auto& instance = vInstances.at(currentInstance);
    auto uniformBlock = instance->mBuffers.find(name);
    if(uniformBlock != instance->mBuffers.end())
        return uniformBlock->second;
    return pEmptyHamdler; 
}

utl::ref_ptr<CPushHandler>& CMaterialBase::getPushConstant(const std::string& name) 
{ 
    auto& instance = vInstances.at(currentInstance);
    auto pushBlock = instance->mPushConstants.find(name);
    if(pushBlock != instance->mPushConstants.end())
        return pushBlock->second;
    return pEmptyPushConstant;
}