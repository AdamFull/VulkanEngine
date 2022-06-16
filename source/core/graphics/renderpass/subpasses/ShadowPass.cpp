#include "ShadowPass.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/SceneManager.h"

using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;
using namespace engine::resources::material;

void CShadowPass::create()
{
    pMaterial = CMaterialLoader::inst()->create("pbr_composition");
    pMaterial->create();
    CSubpass::create();
}

void CShadowPass::reCreate()
{
    pMaterial->reCreate();
}

void CShadowPass::render(vk::CommandBuffer& commandBuffer)
{
    CVBO::inst()->bind(commandBuffer);
    pMaterial->bind(commandBuffer);
    CSceneManager::inst()->getScene()->getRoot()->render(commandBuffer);
}

void CShadowPass::cleanup()
{
    CSubpass::cleanup();
}