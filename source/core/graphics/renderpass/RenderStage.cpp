#include "RenderStage.h"
#include "graphics/VulkanDevice.hpp"

using namespace Engine::Core;
using namespace Engine::Core::Render;

void CRenderStage::render(vk::CommandBuffer& commandBuffer)
{
    pRenderPass->render(commandBuffer);
}

void CRenderStage::cleanup()
{
    pRenderPass->cleanup();
    pFramebuffer->cleanup();
}

void CRenderStage::reCreate()
{
    screenExtent = CDevice::inst()->getExtent();
    pRenderPass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
    pRenderPass->reCreate();
    pFramebuffer->reCreate(pRenderPass->get(), screenExtent);
}