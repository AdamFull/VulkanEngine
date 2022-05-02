#include "FinalCompositionPass.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "resources/materials/MaterialPostProcess.h"
#include "graphics/VulkanInitializers.h"
#include "GlobalVariables.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene::Objects;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

void CFinalCompositionPass::create(std::unique_ptr<FRenderCreateInfo>& createInfo)
{
    auto framesInFlight = USwapChain->GetFramesInFlight();
    pUniform = std::make_shared<CUniformBuffer>();
    pUniform->create(framesInFlight, sizeof(FPostProcess));

    pMaterial = std::make_shared<CMaterialPostProcess>();
    pMaterial->create(createInfo->renderPass, createInfo->subpass);
    CSubpass::create(createInfo);
}

void CFinalCompositionPass::render(std::unique_ptr<FRenderProcessInfo>& renderData)
{
    auto imageIndex = USwapChain->GetCurrentFrame();
    //May be move to CompositionObject
    FPostProcess ubo;
    //HDR
    ubo.gamma = GlobalVariables::postprocessGamma;
    ubo.exposure = GlobalVariables::postprocessExposure;

    pUniform->updateUniformBuffer(imageIndex, &ubo);
    auto& buffer = pUniform->getUniformBuffer(imageIndex);
    auto descriptor = buffer->getDscriptor();
    pMaterial->update(descriptor, imageIndex);
    pMaterial->bind(renderData->commandBuffer, imageIndex);

    renderData->commandBuffer.draw(3, 1, 0, 0);
    //UOverlay->DrawFrame(renderData.commandBuffer, imageIndex);
}

void CFinalCompositionPass::cleanup()
{
    CSubpass::cleanup();
}