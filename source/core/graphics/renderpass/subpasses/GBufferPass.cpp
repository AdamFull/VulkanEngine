#include "GBufferPass.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene::Objects;
using namespace Engine::Resources;

void CGBufferPass::create(std::shared_ptr<FRenderCreateInfo> createData)
{
    createData->root->Create(createData->renderPass, createData->subpass);
    CSubpass::create(createData);
}

void CGBufferPass::render(std::shared_ptr<FRenderProcessInfo> renderData)
{
    auto imageIndex = USwapChain->GetCurrentFrame();
    UVBO->Bind(renderData->commandBuffer);
    renderData->root->Render(renderData->commandBuffer, imageIndex);
}

void CGBufferPass::cleanup()
{
    CSubpass::cleanup();
}