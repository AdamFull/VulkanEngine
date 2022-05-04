#include "GBufferPass.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/SceneManager.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;

void CGBufferPass::create()
{
    auto& renderPass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->get();
    auto subpass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->getCurrentSubpass();
    CSceneManager::getInstance()->getScene()->getRoot()->create(renderPass, subpass);
    CSubpass::create();
}

void CGBufferPass::render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = CSwapChain::getInstance()->getCurrentFrame();
    CVBO::getInstance()->bind(commandBuffer);
    CSceneManager::getInstance()->getScene()->getRoot()->render(commandBuffer, imageIndex);
}

void CGBufferPass::cleanup()
{
    CSubpass::cleanup();
}