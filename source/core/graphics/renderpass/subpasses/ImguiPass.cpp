#include "ImguiPass.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

void CImguiPass::create()
{
    auto& renderPass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->get();
    auto subpass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->getCurrentSubpass();
    CImguiOverlay::getInstance()->create(renderPass, subpass);
    CSubpass::create();
}

void CImguiPass::render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = CSwapChain::getInstance()->getCurrentFrame();
    CImguiOverlay::getInstance()->newFrame();
    CImguiOverlay::getInstance()->drawFrame(commandBuffer, imageIndex);
}

void CImguiPass::cleanup()
{
    CSubpass::cleanup();
}