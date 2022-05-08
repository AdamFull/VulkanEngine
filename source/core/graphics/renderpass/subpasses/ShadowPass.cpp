#include "ShadowPass.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/SceneManager.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

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
    auto imageIndex = CDevice::inst()->getCurrentFrame();
    CVBO::inst()->bind(commandBuffer);
    pMaterial->bind(commandBuffer, imageIndex);
    CSceneManager::inst()->getScene()->getRoot()->render(commandBuffer, imageIndex);
}

void CShadowPass::cleanup()
{
    CSubpass::cleanup();
}