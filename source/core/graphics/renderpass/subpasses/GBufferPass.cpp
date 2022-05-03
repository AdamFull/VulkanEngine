#include "GBufferPass.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;

void CGBufferPass::create(std::shared_ptr<Resources::CResourceManager>& resourceManager, std::shared_ptr<Scene::CRenderObject>& root)
{
    auto& renderPass = URenderer->getCurrentStage()->getRenderPass()->get();
    auto subpass = URenderer->getCurrentStage()->getRenderPass()->getCurrentSubpass();
    root->create(renderPass, subpass);
    CSubpass::create(resourceManager, root);
}

void CGBufferPass::render(vk::CommandBuffer& commandBuffer, std::shared_ptr<Scene::CRenderObject>& root)
{
    auto imageIndex = USwapChain->getCurrentFrame();
    UVBO->bind(commandBuffer);
    root->render(commandBuffer, imageIndex);
}

void CGBufferPass::cleanup()
{
    CSubpass::cleanup();
}