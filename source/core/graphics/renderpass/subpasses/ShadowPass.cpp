#include "ShadowPass.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "resources/materials/MaterialShadow.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

void CShadowPass::create(std::shared_ptr<Resources::CResourceManager>& resourceManager, std::shared_ptr<Scene::CRenderObject>& root, vk::RenderPass& renderPass, uint32_t subpass)
{
    pMaterial = std::make_shared<CMaterialShadow>();
    CSubpass::create(resourceManager, root, renderPass, subpass);
}

void CShadowPass::render(vk::CommandBuffer& commandBuffer, std::unordered_map<std::string, std::shared_ptr<CImage>>& images, std::shared_ptr<Scene::CRenderObject>& root)
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