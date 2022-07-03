#include "PushHandler.hpp"
#include "graphics/VulkanHighLevel.h"
#include "graphics/buffer/VulkanBuffer.h"

using namespace engine::core;
using namespace engine::core::pipeline;

CPushHandler::~CPushHandler()
{
    cleanup();
}

void CPushHandler::create(const CPushConstBlock &uniformBlock, utl::ref_ptr<CPipelineBase>& pipeline)
{
    uint32_t images = CDevice::inst()->getFramesInFlight();

    pPipeline = pipeline;

    for(auto i = 0; i < images; i++)
        vData.emplace_back(utl::make_scope<char[]>(uniformBlock.getSize()));
    pushBlock = uniformBlock;
}

void CPushHandler::reCreate(utl::ref_ptr<CPipelineBase>& pipeline)
{
    create(pushBlock.value(), pipeline);
}

void CPushHandler::cleanup()
{
    if(!bIsClean)
    {
        for(auto& buffer : vBuffers)
            buffer->cleanup();
        vBuffers.clear();
        vData.clear();
        bIsClean = true;
    }
}

void CPushHandler::flush(vk::CommandBuffer& commandBuffer)
{
    uint32_t index = CDevice::inst()->getCurrentFrame();
    
    auto& data = vData.at(index);
    if(data && pPipeline)
    {
        commandBuffer.pushConstants(pPipeline->getPipelineLayout(), pushBlock->getStageFlags(), 0, static_cast<uint32_t>(pushBlock->getSize()), data.get());
    }
}

uint32_t CPushHandler::getCurrentFrameProxy()
{
    return CDevice::inst()->getCurrentFrame();
}