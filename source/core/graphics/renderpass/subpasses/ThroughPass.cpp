#include "ThroughPass.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/materials/MaterialLoader.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

void CThroughPass::create()
{
    auto& renderPass = CRenderSystem::inst()->getCurrentStage()->getRenderPass()->get();
    auto subpass = CRenderSystem::inst()->getCurrentStage()->getRenderPass()->getCurrentSubpass();
    pMaterial = CMaterialLoader::inst()->create("through_pass");
    pMaterial->create(renderPass, subpass);
    CSubpass::create();
}

void CThroughPass::render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = CSwapChain::inst()->getCurrentFrame();
    pMaterial->addTexture("samplerColor", CRenderSystem::inst()->getPrevStage()->getFramebuffer()->getCurrentImages()["output_color"]);

    pMaterial->update(imageIndex);
    pMaterial->bind(commandBuffer, imageIndex);

    commandBuffer.draw(3, 1, 0, 0);
}

void CThroughPass::cleanup()
{
    CSubpass::cleanup();
}