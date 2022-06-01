#include "RenderStage.h"
#include "graphics/VulkanDevice.hpp"

using namespace engine::core;
using namespace engine::core::render;

void CRenderStage::create()
{
    framebufferIndex = 0;
    for(auto& framebuffer : vFramebuffer)
    {
        framebuffer->create();
        framebufferIndex++;
    }
}

void CRenderStage::render(vk::CommandBuffer& commandBuffer)
{
    framebufferIndex = 0;
    for(auto& framebuffer : vFramebuffer)
    {
        framebuffer->render(commandBuffer);
        framebufferIndex++;
    }
}

void CRenderStage::cleanup()
{
    framebufferIndex = 0;
    for(auto& framebuffer : vFramebuffer)
    {
        framebuffer->cleanup();
        framebufferIndex++;
    }
}

void CRenderStage::reCreate()
{
    screenExtent = CDevice::inst()->getExtent(detectExtent);
    framebufferIndex = 0;
    for(auto& framebuffer : vFramebuffer)
    {
        framebuffer->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
        framebuffer->reCreate();
        framebufferIndex++;
    }
}