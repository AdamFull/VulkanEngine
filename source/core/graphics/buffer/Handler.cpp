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
}

void CHandler::reCreate()
{
    create(uniformBlock.value());
}

void CHandler::cleanup()
{
    if(!bIsClean)
    {
        if(bMapped)
            pBuffer->unmapMem();
        pBuffer->cleanup();
        bIsClean = true;
    }
}

void CHandler::flush()
{
    uint32_t index = CDevice::inst()->getCurrentFrame();
    if (!pBuffer)
		return;
    
    if(status != EHandlerStatus::eFlushed)
    {
        if(bMapped)
        {
            pBuffer->unmapMem();
            bMapped = false;
        }

        status == EHandlerStatus::eFlushed;
    }
}