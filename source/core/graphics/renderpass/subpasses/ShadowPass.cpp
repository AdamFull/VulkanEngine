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
    pMaterial = CMaterialLoader::getInstance()->create("pbr_composition");
    auto& renderPass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->get();
    auto subpass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->getCurrentSubpass();
    pMaterial->create(renderPass, subpass);
    CSubpass::create();
}

void CShadowPass::render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = CSwapChain::getInstance()->getCurrentFrame();
    CVBO::getInstance()->bind(commandBuffer);
    pMaterial->bind(commandBuffer, imageIndex);
    CSceneManager::getInstance()->getScene()->getRoot()->render(commandBuffer, imageIndex);
}

void CShadowPass::cleanup()
{
    CSubpass::cleanup();
}