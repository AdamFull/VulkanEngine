#include "RenderStage.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;

void CRenderStage::render(vk::CommandBuffer& commandBuffer, std::shared_ptr<Scene::CRenderObject>& root)
{
    pRenderPass->render(commandBuffer, root);
}

void CRenderStage::cleanup()
{
    pRenderPass->cleanup();
    pFramebuffer->cleanup();
}