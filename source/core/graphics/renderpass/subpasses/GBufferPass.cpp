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
    UScene->getScene()->getRoot()->create();
    CSubpass::create();
}

void CGBufferPass::reCreate()
{
    UScene->getScene()->getRoot()->reCreate();
}

void CGBufferPass::render(vk::CommandBuffer& commandBuffer)
{
    URenderer->setStageType(EStageType::eDeferred);
    UVBO->bind(commandBuffer);
    UScene->getScene()->getRoot()->render(commandBuffer);
}