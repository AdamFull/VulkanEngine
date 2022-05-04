#include "RenderStage.h"

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