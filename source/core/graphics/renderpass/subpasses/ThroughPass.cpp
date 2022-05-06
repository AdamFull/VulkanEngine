#include "ThroughPass.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/materials/MaterialLoader.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

void CThroughPass::create()
{
    auto& renderPass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->get();
    auto subpass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->getCurrentSubpass();
    pMaterial = CMaterialLoader::getInstance()->create("through_pass");
    pMaterial->create(renderPass, subpass);
    CSubpass::create();
}

void CThroughPass::render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = CSwapChain::getInstance()->getCurrentFrame();
    pMaterial->addTexture("samplerColor", CRenderSystem::getInstance()->getPrevStage()->getFramebuffer()->getCurrentImages()["output_color"]);

    pMaterial->update(imageIndex);
    pMaterial->bind(commandBuffer, imageIndex);

    commandBuffer.draw(3, 1, 0, 0);
}

void CThroughPass::cleanup()
{
    CSubpass::cleanup();
}