#include "ImguiPass.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

void CImguiPass::create()
{
    auto& renderPass = CRenderSystem::inst()->getCurrentStage()->getRenderPass()->get();
    auto subpass = CRenderSystem::inst()->getCurrentStage()->getRenderPass()->getCurrentSubpass();
    CEditorUI::inst()->create(renderPass, subpass);
    CSubpass::create();
}

void CImguiPass::reCreate()
{
    CEditorUI::inst()->reCreate();
}

void CImguiPass::render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = CDevice::inst()->getCurrentFrame();
    CEditorUI::inst()->newFrame();
    CEditorUI::inst()->drawFrame(commandBuffer, imageIndex);
}

void CImguiPass::cleanup()
{
    CSubpass::cleanup();
}