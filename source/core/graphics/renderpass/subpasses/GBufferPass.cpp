#include "GBufferPass.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/SceneManager.h"

using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;

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
    auto imageIndex = CDevice::inst()->getCurrentFrame();
    CVBO::inst()->bind(commandBuffer);
    CSceneManager::inst()->getScene()->getRoot()->render(commandBuffer, imageIndex);
}

void CGBufferPass::cleanup()
{
    CSceneManager::inst()->getScene()->cleanup();
    CSubpass::cleanup();
}