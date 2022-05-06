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
    CSceneManager::inst()->getScene()->getRoot()->create();
    CSubpass::create();
}

void CGBufferPass::reCreate()
{
    CSceneManager::inst()->getScene()->getRoot()->reCreate();
}

void CGBufferPass::render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = CSwapChain::inst()->getCurrentFrame();
    CVBO::inst()->bind(commandBuffer);
    CSceneManager::inst()->getScene()->getRoot()->render(commandBuffer, imageIndex);
}

void CGBufferPass::cleanup()
{
    CSubpass::cleanup();
}