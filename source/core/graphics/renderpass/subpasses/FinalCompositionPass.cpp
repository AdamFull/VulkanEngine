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

void CFinalCompositionPass::create(std::shared_ptr<FRenderCreateInfo> createData)
{
    auto framesInFlight = USwapChain->GetFramesInFlight();
    m_pUniform = std::make_shared<UniformBuffer>();
    m_pUniform->Create(framesInFlight, sizeof(FPostProcess));

    m_pMaterial = std::make_shared<MaterialPostProcess>();
    m_pMaterial->Create(createData->renderPass, createData->subpass);
    CSubpass::create(createData);
}

void CFinalCompositionPass::render(std::shared_ptr<FRenderProcessInfo> renderData)
{
    auto imageIndex = USwapChain->GetCurrentFrame();
    //May be move to CompositionObject
    FPostProcess ubo;
    //HDR
    ubo.gamma = GlobalVariables::postprocessGamma;
    ubo.exposure = GlobalVariables::postprocessExposure;

    m_pUniform->UpdateUniformBuffer(imageIndex, &ubo);
    auto& buffer = m_pUniform->GetUniformBuffer(imageIndex);
    auto descriptor = buffer->GetDscriptor();
    m_pMaterial->Update(descriptor, imageIndex);
    m_pMaterial->Bind(renderData->commandBuffer, imageIndex);

    renderData->commandBuffer.draw(3, 1, 0, 0);
    //UOverlay->DrawFrame(renderData.commandBuffer, imageIndex);
}

void CFinalCompositionPass::cleanup()
{
    CSubpass::cleanup();
}