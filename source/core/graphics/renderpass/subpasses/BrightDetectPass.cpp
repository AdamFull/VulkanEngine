#include "BrightDetectPass.h"
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
    pMaterial->bind();
    commandBuffer.draw(3, 1, 0, 0);
}

void CBrightDetectPass::cleanup()
{
    CSubpass::cleanup();
}