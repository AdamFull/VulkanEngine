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
    auto framesInFlight = CDevice::inst()->getFramesInFlight();

    auto& renderPass = CRenderSystem::inst()->getCurrentStage()->getRenderPass()->get();
    auto subpass = CRenderSystem::inst()->getCurrentStage()->getRenderPass()->getCurrentSubpass();
//VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
    pImage = CFramebuffer::createImage(vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled, CDevice::inst()->getExtent());

    pMaterial = CMaterialLoader::inst()->create("gaussian_blur");
    pMaterial->create();
    CSubpass::create();
}

void CGaussianBlurPass::reCreate()
{
    pMaterial->reCreate();
}

void CGaussianBlurPass::render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = CDevice::inst()->getCurrentFrame();

    pMaterial->addTexture("writeColour", pImage);
    pMaterial->addTexture("samplerBrightness", CRenderSystem::inst()->getPrevStage()->getFramebuffer()->getCurrentImages()[imageReferenceName]);

    auto pUBO = pMaterial->getUniformBuffer("FBloomUbo");
    pUBO->set("blurScale", &GlobalVariables::blurScale, imageIndex);
    pUBO->set("blurStrength", &GlobalVariables::blurStrength, imageIndex);
    pUBO->set("direction", &direction, imageIndex);
    
    pMaterial->update(imageIndex);
    pMaterial->bind(commandBuffer, imageIndex);
    commandBuffer.draw(3, 1, 0, 0);
}

void CGaussianBlurPass::cleanup()
{
    CSubpass::cleanup();
}