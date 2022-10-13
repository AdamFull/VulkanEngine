#include "VoxelPass.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/materials/MaterialLoader.h"

using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;
using namespace engine::resources::material;

void CVoxelPass::create()
{
    pMaterial = CMaterialLoader::inst()->create("voxel_pass");
    pMaterial->create();
    CSubpass::create();
}

void CVoxelPass::reCreate()
{
    pMaterial->reCreate();
}

void CVoxelPass::render(vk::CommandBuffer& commandBuffer)
{
    URenderer->setStageType(EStageType::eDeferred);
    auto imageIndex = UDevice->getCurrentFrame();
    //pMaterial->addTexture("samplerColor", URenderer->getCurrentImages()[srImageSource]);

    pMaterial->update();
    pMaterial->bind(commandBuffer);

    commandBuffer.draw(3, 1, 0, 0);
}