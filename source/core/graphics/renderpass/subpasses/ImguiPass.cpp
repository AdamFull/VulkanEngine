#include "ImguiPass.h"
#include "graphics/VulkanHighLevel.h"

using namespace engine::editor;
using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;
using namespace engine::resources::material;

void CImguiPass::create()
{
    auto& renderPass = CRenderSystem::inst()->getCurrentStage()->getCurrentFramebuffer()->getRenderPass();
    auto subpass = CRenderSystem::inst()->getCurrentStage()->getCurrentFramebuffer()->getCurrentSubpass();
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