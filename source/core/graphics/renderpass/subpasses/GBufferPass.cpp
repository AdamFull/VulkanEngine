#include "GBufferPass.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene::Objects;
using namespace Engine::Resources;

void CGBufferPass::create(std::shared_ptr<ResourceManager> resourceManager, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<RenderObject> root, vk::RenderPass& renderPass, uint32_t subpass)
{
    root->Create(renderPass, subpass);
}

void CGBufferPass::render(vk::CommandBuffer& commandBuffer, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<RenderObject> root)
{
    auto imageIndex = USwapChain->GetCurrentFrame();
    UVBO->Bind(commandBuffer);
    root->Render(commandBuffer, imageIndex);
}

void CGBufferPass::cleanup()
{
    CSubpass::cleanup();
}