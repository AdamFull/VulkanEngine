#include "GaussianBlurPass.h"
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

void CGaussianBlurPass::create()
{
    auto framesInFlight = CSwapChain::getInstance()->getFramesInFlight();
    pUniform = std::make_shared<CUniformBuffer>();
    pUniform->create(framesInFlight, sizeof(FBlurData));

    auto& renderPass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->get();
    auto subpass = CRenderSystem::getInstance()->getCurrentStage()->getRenderPass()->getCurrentSubpass();

    pMaterial = CMaterialLoader::getInstance()->create("gaussian_blur");
    pMaterial->create(renderPass, subpass);
    CSubpass::create();
}

void CGaussianBlurPass::render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = CSwapChain::getInstance()->getCurrentFrame();

    if(direction < 0)
        pMaterial->addTexture("samplerBrightness", CRenderSystem::getInstance()->getPrevStage()->getFramebuffer()->getCurrentImages()[imageReferenceName]);
    else
        pMaterial->addTexture("samplerBrightness", CRenderSystem::getInstance()->getPrevStage()->getFramebuffer()->getCurrentImages()[imageReferenceName]);

    FBlurData uniform;
    uniform.blurScale = GlobalVariables::blurScale;
    uniform.blurStrength = GlobalVariables::blurStrength;
    uniform.direction = direction;

    pUniform->updateUniformBuffer(imageIndex, &uniform);
    auto& buffer = pUniform->getUniformBuffer(imageIndex);
    auto descriptor = buffer->getDscriptor();
    pMaterial->addBuffer("FBloomUbo", descriptor);
    pMaterial->update(imageIndex);
    pMaterial->bind(commandBuffer, imageIndex);
    commandBuffer.draw(3, 1, 0, 0);
}

void CGaussianBlurPass::cleanup()
{
    CSubpass::cleanup();
}