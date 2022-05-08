#include "PushHandler.hpp"
#include "graphics/VulkanHighLevel.h"
#include "graphics/buffer/VulkanBuffer.h"

using namespace Engine::Core;
using namespace Engine::Core::Pipeline;

CPushHandler::~CPushHandler()
{
    cleanup();
}

void CPushHandler::create(const CPushConstBlock &uniformBlock)
{
    uint32_t images = CDevice::inst()->getFramesInFlight();
    for(auto i = 0; i < images; i++)
        vData.emplace_back(std::make_unique<char[]>(uniformBlock.getSize()));
    pushBlock = uniformBlock;
}

void CPushHandler::reCreate()
{
    create(pushBlock.value());
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

void CPushHandler::flush(vk::CommandBuffer& commandBuffer, std::shared_ptr<Pipeline::CPipelineBase> pPipeline)
{
    uint32_t index = CDevice::inst()->getCurrentFrame();
    
    auto& data = vData.at(index);
    if(data)
    {
        commandBuffer.pushConstants(pPipeline->getPipelineLayout(), pushBlock->getStageFlags(), 0, static_cast<uint32_t>(pushBlock->getSize()), data.get());
    }
}
