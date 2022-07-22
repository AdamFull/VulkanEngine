#include "RenderStage.h"
#include "graphics/VulkanHighLevel.h"

using namespace engine::core;
using namespace engine::core::render;

CRenderStage::~CRenderStage()
{
    vFramebuffer.clear();
}

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

void CRenderStage::reCreate()
{
    screenExtent = UDevice->getExtent(detectExtent);
    framebufferIndex = 0;
    for(auto& framebuffer : vFramebuffer)
    {
        framebuffer->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
        framebuffer->reCreate();
        framebufferIndex++;
    }
}