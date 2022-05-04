#include "GBufferPass.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;

void CGBufferPass::create(std::shared_ptr<Scene::CRenderObject>& root)
{
    auto& renderPass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->get();
    auto subpass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->getCurrentSubpass();
    root->create(renderPass, subpass);
    CSubpass::create(root);
}

void CGBufferPass::render(vk::CommandBuffer& commandBuffer, std::shared_ptr<Scene::CRenderObject>& root)
{
    auto imageIndex = CSwapChain::getInstance()->getCurrentFrame();
    CVBO::getInstance()->bind(commandBuffer);
    root->render(commandBuffer, imageIndex);
}

void CGBufferPass::cleanup()
{
    CSubpass::cleanup();
}