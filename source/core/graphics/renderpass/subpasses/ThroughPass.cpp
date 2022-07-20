#include "ThroughPass.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/materials/MaterialLoader.h"

using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;
using namespace engine::resources::material;

void CThroughPass::create()
{
    pMaterial = CMaterialLoader::inst()->create("through_pass");
    pMaterial->create();
    CSubpass::create();
}

void CThroughPass::reCreate()
{
    pMaterial->reCreate();
}

void CThroughPass::render(vk::CommandBuffer& commandBuffer)
{
    URenderer->setStageType(EStageType::ePresent);
    auto imageIndex = UDevice->getCurrentFrame();
    pMaterial->addTexture("samplerColor", URenderer->getCurrentImages()[srImageSource]);

    pMaterial->update();
    pMaterial->bind(commandBuffer);

    commandBuffer.draw(3, 1, 0, 0);
}