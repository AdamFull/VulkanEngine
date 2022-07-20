#include "ImguiPass.h"
#include "graphics/VulkanHighLevel.h"

using namespace engine::editor;
using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;
using namespace engine::resources::material;

void CImguiPass::create()
{
    auto& renderPass = URenderer->getCurrentStage()->getCurrentFramebuffer()->getRenderPass();
    auto subpass = URenderer->getCurrentStage()->getCurrentFramebuffer()->getCurrentSubpass();
    UEditorUI->create(renderPass, subpass);
    CSubpass::create();
}

void CImguiPass::reCreate()
{
    UEditorUI->reCreate();
}

void CImguiPass::render(vk::CommandBuffer& commandBuffer)
{
    URenderer->setStageType(EStageType::ePresent);
    auto imageIndex = UDevice->getCurrentFrame();
    UEditorUI->newFrame();
    UEditorUI->drawFrame(commandBuffer, imageIndex);
}