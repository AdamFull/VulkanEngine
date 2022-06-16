#include "DownsamplePass.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "GlobalVariables.h"

using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;
using namespace engine::resources::material;

void CDownsamplePass::create()
{
    auto framesInFlight = CDevice::inst()->getFramesInFlight();

    pMaterial = CMaterialLoader::inst()->create("downsample");
    pMaterial->create();
    CSubpass::create();
}

void CDownsamplePass::reCreate()
{
    pMaterial->reCreate();
}

void CDownsamplePass::render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = CDevice::inst()->getCurrentFrame();

    pMaterial->addTexture("downsample_tex", CRenderSystem::inst()->getCurrentImages()[targetTextureName]);

    pMaterial->update();
    pMaterial->bind(commandBuffer);
    commandBuffer.draw(3, 1, 0, 0);
}

void CDownsamplePass::cleanup()
{
    CSubpass::cleanup();
}