#include "ShadowPass.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

void CShadowPass::create(std::shared_ptr<Resources::CResourceManager>& resourceManager, std::shared_ptr<Scene::CRenderObject>& root)
{
    pMaterial = CMaterialLoader::getInstance()->create("pbr_composition");
    auto& renderPass = URenderer->getCurrentStage()->getRenderPass()->get();
    auto subpass = URenderer->getCurrentStage()->getRenderPass()->getCurrentSubpass();
    pMaterial->create(renderPass, subpass);
    CSubpass::create(resourceManager, root);
}

void CShadowPass::render(vk::CommandBuffer& commandBuffer, std::shared_ptr<Scene::CRenderObject>& root)
{
    auto imageIndex = USwapChain->getCurrentFrame();
    UVBO->bind(commandBuffer);
    pMaterial->bind(commandBuffer, imageIndex);
    root->render(commandBuffer, imageIndex);
}

void CShadowPass::cleanup()
{
    CSubpass::cleanup();
}