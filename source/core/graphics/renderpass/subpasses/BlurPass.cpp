#include "BlurPass.h"
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

void CBlurPass::create()
{
    auto framesInFlight = CDevice::inst()->getFramesInFlight();

    pMaterial = CMaterialLoader::inst()->create("gaussian_blur");
    pMaterial->create();
    CSubpass::create();
}

void CBlurPass::reCreate()
{
    pMaterial->reCreate();
}

void CBlurPass::render(vk::CommandBuffer& commandBuffer)
{
    CRenderSystem::inst()->setStageType(EStageType::ePostProcess);
    auto imageIndex = CDevice::inst()->getCurrentFrame();

    pMaterial->addTexture("samplerBrightness", CRenderSystem::inst()->getCurrentImages()[imageReferenceName]);

    auto& pUBOH = pMaterial->getPushConstant("ubo");
    pUBOH->set("blurScale", GlobalVariables::blurScale);
    pUBOH->set("blurStrength", GlobalVariables::blurStrength);
    pUBOH->set("direction", direction);
    pUBOH->flush(commandBuffer);

    pMaterial->update();
    pMaterial->bind(commandBuffer);
    commandBuffer.draw(3, 1, 0, 0);
}

void CBlurPass::cleanup()
{
    CSubpass::cleanup();
}