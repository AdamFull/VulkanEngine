#include "ShadowPass.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "resources/materials/MaterialShadow.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene::Objects;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

void CShadowPass::create(std::shared_ptr<ResourceManager> resourceManager, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<RenderObject> root, vk::RenderPass& renderPass, uint32_t subpass)
{
    m_pMaterial = std::make_shared<MaterialShadow>();
}

void CShadowPass::render(vk::CommandBuffer& commandBuffer, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<RenderObject> root)
{
    auto imageIndex = USwapChain->GetCurrentFrame();
    UVBO->Bind(commandBuffer);
    m_pMaterial->Bind(commandBuffer, imageIndex);
    root->Render(commandBuffer, imageIndex);
}

void CShadowPass::cleanup()
{
    CSubpass::cleanup();
}