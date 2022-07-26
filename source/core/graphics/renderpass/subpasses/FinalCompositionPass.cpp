#include "FinalCompositionPass.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/VulkanInitializers.h"
#include "GlobalVariables.h"

using namespace engine::core::window;
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
    auto& images = URenderer->getCurrentImages();
    auto imageIndex = UDevice->getCurrentFrame();

    pMaterial->addTexture("samplerColor", images["composition_tex"]);
    //pMaterial->addTexture("velocity_tex", images["velocity_tex"]);
    pMaterial->addTexture("samplerBrightness", images[blurImageSample]); //bloom_image
    //May be move to CompositionObject

    //TODO: push constant ranges here
    auto& pUBO = pMaterial->getPushConstant("ubo");
    pUBO->set("enableFXAA", GlobalVariables::enableFXAA);
    pUBO->set("texelStep", glm::vec2(1.0 / CWindowHandle::m_iWidth, 1.0 / CWindowHandle::m_iWidth));
    pUBO->flush(commandBuffer);

    auto& pUBOTonemap = pMaterial->getUniformBuffer("UBOTonemap");
    pUBOTonemap->set("gamma", GlobalVariables::postprocessGamma);
    pUBOTonemap->set("exposure", GlobalVariables::postprocessExposure);

    auto& pUBOFXAA = pMaterial->getUniformBuffer("UBOFXAA");
    pUBOFXAA->set("qualitySubpix", GlobalVariables::qualitySubpix);
    pUBOFXAA->set("qualityEdgeThreshold", GlobalVariables::qualityEdgeThreshold);
    pUBOFXAA->set("qualityEdgeThresholdMin", GlobalVariables::qualityEdgeThresholdMin);

    auto& pUBOMotionBlur = pMaterial->getUniformBuffer("UBOMotionBlur");
    //pUBOMotionBlur->set("velocityScale", GlobalVariables::framerate / 60.f);

    pMaterial->update();
    pMaterial->bind(commandBuffer);

    commandBuffer.draw(3, 1, 0, 0);
}