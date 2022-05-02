#include "GBufferPass.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;

void CGBufferPass::create(std::shared_ptr<Resources::CResourceManager>& resourceManager, std::shared_ptr<Scene::CRenderObject>& root, vk::RenderPass& renderPass, uint32_t subpass)
{
    root->create(renderPass, subpass);
    CSubpass::create(resourceManager, root, renderPass, subpass);
}

void CGBufferPass::render(vk::CommandBuffer& commandBuffer, std::unordered_map<std::string, std::shared_ptr<CImage>>& images, std::shared_ptr<Scene::CRenderObject>& root)
{
    auto imageIndex = USwapChain->getCurrentFrame();
    UVBO->bind(commandBuffer);
    root->render(commandBuffer, imageIndex);
}

void CGBufferPass::cleanup()
{
    CSubpass::cleanup();
}