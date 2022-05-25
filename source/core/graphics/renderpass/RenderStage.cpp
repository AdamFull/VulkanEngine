#include "RenderStage.h"
#include "graphics/VulkanDevice.hpp"

using namespace Engine::Core;
using namespace Engine::Core::Render;

void CRenderStage::render(vk::CommandBuffer& commandBuffer)
{
    pFramebuffer->render(commandBuffer);
}

void CRenderStage::cleanup()
{
    pFramebuffer->cleanup();
}

void CRenderStage::reCreate()
{
    screenExtent = CDevice::inst()->getExtent(detectExtent);
    pFramebuffer->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pFramebuffer->reCreate();
}