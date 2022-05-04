#include "FinalCompositionPass.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/VulkanInitializers.h"
#include "GlobalVariables.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

void CFinalCompositionPass::create()
{
    auto framesInFlight = CSwapChain::getInstance()->getFramesInFlight();
    pUniform = std::make_shared<CUniformBuffer>();
    pUniform->create(framesInFlight, sizeof(FPostProcess));

    auto& renderPass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->get();
    auto subpass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->getCurrentSubpass();

    pMaterial = CMaterialLoader::getInstance()->create("post_process");
    pMaterial->create(renderPass, subpass);
    CImguiOverlay::getInstance()->create(renderPass, subpass);
    CSubpass::create();
}

void CFinalCompositionPass::render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = CSwapChain::getInstance()->getCurrentFrame();

    pMaterial->addTexture("samplerColor", CRenderSystem::getInstance()->getPrevStage()->getFramebuffer()->getCurrentImages()["output_color"]);
    pMaterial->addTexture("samplerBrightness", CRenderSystem::getInstance()->getPrevStage()->getFramebuffer()->getCurrentImages()["brightness_buffer"]); //bloom_image
    //May be move to CompositionObject
    FPostProcess ubo;
    //HDR
    ubo.gamma = GlobalVariables::postprocessGamma;
    ubo.exposure = GlobalVariables::postprocessExposure;

    pUniform->updateUniformBuffer(imageIndex, &ubo);
    auto& buffer = pUniform->getUniformBuffer(imageIndex);
    auto descriptor = buffer->getDscriptor();
    pMaterial->addBuffer("FBloomUbo", descriptor);
    pMaterial->update(imageIndex);
    pMaterial->bind(commandBuffer, imageIndex);

    commandBuffer.draw(3, 1, 0, 0);
    CImguiOverlay::getInstance()->drawFrame(commandBuffer, imageIndex);
}

void CFinalCompositionPass::cleanup()
{
    CSubpass::cleanup();
}