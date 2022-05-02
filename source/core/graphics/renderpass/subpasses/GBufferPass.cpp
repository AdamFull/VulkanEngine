#include "GBufferPass.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;

void CGBufferPass::create(std::unique_ptr<FRenderCreateInfo>& createInfo)
{
    createInfo->root->create(createInfo->renderPass, createInfo->subpass);
    CSubpass::create(createInfo);
}

void CGBufferPass::render(std::unique_ptr<FRenderProcessInfo>& renderData)
{
    auto imageIndex = USwapChain->getCurrentFrame();
    UVBO->bind(renderData->commandBuffer);
    renderData->root->render(renderData->commandBuffer, imageIndex);
}

void CGBufferPass::cleanup()
{
    CSubpass::cleanup();
}