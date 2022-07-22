#include "PushHandler.hpp"
#include "graphics/VulkanHighLevel.h"
#include "graphics/buffer/VulkanBuffer.h"

using namespace engine::core;
using namespace engine::core::pipeline;

CPushHandler::~CPushHandler()
{
    vData.clear();
}

CPushHandler::CPushHandler(const pipeline::CPushConstBlock &pushBlock, utl::scope_ptr<pipeline::CPipelineBase>& pipeline)
{
    create(pushBlock, pipeline);
}

void CPushHandler::create(const CPushConstBlock &uniformBlock, utl::scope_ptr<CPipelineBase>& pipeline)
{
    uint32_t images = UDevice->getFramesInFlight();

    pPipeline = pipeline.get();

    for(auto i = 0; i < images; i++)
        vData.emplace_back(utl::make_scope<char[]>(uniformBlock.getSize()));
    pushBlock = uniformBlock;
}

void CPushHandler::reCreate(utl::scope_ptr<CPipelineBase>& pipeline)
{
    create(pushBlock.value(), pipeline);
}

void CPushHandler::flush(vk::CommandBuffer& commandBuffer)
{
    uint32_t index = UDevice->getCurrentFrame();
    
    auto& data = vData.at(index);
    if(data && pPipeline)
    {
        commandBuffer.pushConstants(pPipeline->getPipelineLayout(), pushBlock->getStageFlags(), 0, static_cast<uint32_t>(pushBlock->getSize()), data.get());
    }
}

uint32_t CPushHandler::getCurrentFrameProxy()
{
    return UDevice->getCurrentFrame();
}