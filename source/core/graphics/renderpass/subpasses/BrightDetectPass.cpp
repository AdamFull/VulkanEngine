#include "BrightDetectPass.h"
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

void CBrightDetectPass::create()
{
    auto framesInFlight = CDevice::inst()->getFramesInFlight();

    pMaterial = CMaterialLoader::inst()->create("brightdetect");
    pMaterial->create();
    CSubpass::create();
}

void CBrightDetectPass::reCreate()
{
    pMaterial->reCreate();
}

void CBrightDetectPass::render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = CDevice::inst()->getCurrentFrame();

    pMaterial->addTexture("composition_tex", CRenderSystem::inst()->getCurrentImages()["composition_tex"]);

    auto& pUBO = pMaterial->getPushConstant("ubo");
    pUBO->set("bloom_threshold", GlobalVariables::bloomThreshold);
    pUBO->flush(commandBuffer);

    pMaterial->update();
    pMaterial->bind(commandBuffer);
    commandBuffer.draw(3, 1, 0, 0);
}

void CBrightDetectPass::cleanup()
{
    CSubpass::cleanup();
}