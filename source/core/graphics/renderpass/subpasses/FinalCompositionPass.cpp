#include "FinalCompositionPass.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/VulkanInitializers.h"
#include "GlobalVariables.h"

using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;
using namespace engine::resources::material;

void CFinalCompositionPass::create()
{
    auto framesInFlight = UDevice->getFramesInFlight();

    pMaterial = CMaterialLoader::inst()->create("post_process");
    pMaterial->create();
    CSubpass::create();
}

void CFinalCompositionPass::reCreate()
{
    pMaterial->reCreate();
}

void CFinalCompositionPass::render(vk::CommandBuffer& commandBuffer)
{
    URenderer->setStageType(EStageType::ePostProcess);
    auto imageIndex = UDevice->getCurrentFrame();

    pMaterial->addTexture("samplerColor", URenderer->getCurrentImages()["composition_tex"]);
    pMaterial->addTexture("samplerBrightness", URenderer->getCurrentImages()[blurImageSample]); //bloom_image
    //May be move to CompositionObject

    //TODO: push constant ranges here
    auto& pUBO = pMaterial->getPushConstant("ubo");
    pUBO->set("gamma", GlobalVariables::postprocessGamma);
    pUBO->set("exposure", GlobalVariables::postprocessExposure);
    pUBO->flush(commandBuffer);

    pMaterial->update();
    pMaterial->bind(commandBuffer);

    commandBuffer.draw(3, 1, 0, 0);
}