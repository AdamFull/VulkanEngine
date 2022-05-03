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

void CFinalCompositionPass::create(std::shared_ptr<Resources::CResourceManager>& resourceManager, std::shared_ptr<Scene::CRenderObject>& root)
{
    auto framesInFlight = USwapChain->getFramesInFlight();
    pUniform = std::make_shared<CUniformBuffer>();
    pUniform->create(framesInFlight, sizeof(FPostProcess));

    auto& renderPass = URenderer->getCurrentStage()->getRenderPass()->get();
    auto subpass = URenderer->getCurrentStage()->getRenderPass()->getCurrentSubpass();

    pMaterial = CMaterialLoader::getInstance()->create("post_process");
    pMaterial->create(renderPass, subpass);
    CSubpass::create(resourceManager, root);
}

void CFinalCompositionPass::render(vk::CommandBuffer& commandBuffer, std::shared_ptr<Scene::CRenderObject>& root)
{
    auto imageIndex = USwapChain->getCurrentFrame();
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
}

void CFinalCompositionPass::cleanup()
{
    CSubpass::cleanup();
}