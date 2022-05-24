#include "BlurPass.h"
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
    auto imageIndex = CDevice::inst()->getCurrentFrame();

    if(direction < 0)
        pMaterial->addTexture("samplerBrightness", CRenderSystem::inst()->getPrevStage()->getFramebuffer()->getCurrentImages()[imageReferenceName]);
    else
        pMaterial->addTexture("samplerBrightness", CRenderSystem::inst()->getCurrentStage()->getFramebuffer()->getCurrentImages()[imageReferenceName]);

    auto& pUBOH = pMaterial->getPushConstant("ubo");
    pUBOH->set("blurScale", GlobalVariables::blurScale);
    pUBOH->set("blurStrength", GlobalVariables::blurStrength);
    pUBOH->set("direction", direction);
    pUBOH->flush(commandBuffer);

    pMaterial->update();
    pMaterial->bind();
    commandBuffer.draw(3, 1, 0, 0);
}

void CBlurPass::cleanup()
{
    CSubpass::cleanup();
}