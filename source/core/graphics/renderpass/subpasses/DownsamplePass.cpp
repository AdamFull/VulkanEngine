#include "DownsamplePass.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "GlobalVariables.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

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
    pMaterial->bind();
    commandBuffer.draw(3, 1, 0, 0);
}

void CDownsamplePass::cleanup()
{
    CSubpass::cleanup();
}