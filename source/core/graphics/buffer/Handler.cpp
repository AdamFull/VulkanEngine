#include "Handler.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Pipeline;

CHandler::~CHandler()
{
    cleanup();
}

void CHandler::create(const CUniformBlock &_uniformBlock)
{
    uniformBlock = _uniformBlock;
    vMapped.resize(CDevice::inst()->getFramesInFlight());
}

void CHandler::reCreate()
{
    create(uniformBlock.value());
}

void CHandler::cleanup()
{
    if(!bIsClean)
    {
        for(auto& buffer : vBuffers)
            buffer->cleanup();
        vBuffers.clear();
        bIsClean = true;
    }
}

void CHandler::flush()
{
    uint32_t index = CDevice::inst()->getCurrentFrame();
    if (vBuffers.empty())
		return;
    
    if(status != EHandlerStatus::eFlushed)
    {
        if(vMapped.at(index))
        {
            vBuffers.at(index)->unmapMem();
            vMapped.at(index) = false;
        }

        status = EHandlerStatus::eFlushed;
    }
}

scope_ptr<CVulkanBuffer>& CHandler::getBuffer()
{
    uint32_t index = CDevice::inst()->getCurrentFrame();
    return vBuffers.at(index);
}

uint32_t CHandler::getCurrentFrameProxy()
{
    return CDevice::inst()->getCurrentFrame();
}