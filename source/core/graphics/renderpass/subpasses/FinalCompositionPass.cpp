#include "FinalCompositionPass.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "resources/ResourceManager.h"
#include "resources/materials/MaterialPostProcess.h"
#include "graphics/VulkanInitializers.h"
#include "GlobalVariables.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene::Objects;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

void CFinalCompositionPass::create(std::shared_ptr<ResourceManager> resourceManager, std::shared_ptr<RenderObject> root, vk::RenderPass& renderPass, uint32_t subpass)
{
    auto framesInFlight = USwapChain->GetFramesInFlight();
    m_pUniform = std::make_shared<UniformBuffer>();
    m_pUniform->Create(framesInFlight, sizeof(FPostProcess));

    m_pMaterial = std::make_shared<MaterialPostProcess>();
    m_pMaterial->Create(renderPass, subpass);
}

void CFinalCompositionPass::render(vk::CommandBuffer& commandBuffer, std::shared_ptr<RenderObject> root)
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
    m_pMaterial->Bind(commandBuffer, imageIndex);

    commandBuffer.draw(3, 1, 0, 0);
    UOverlay->DrawFrame(commandBuffer, imageIndex);
}

void CFinalCompositionPass::cleanup()
{
    CSubpass::cleanup();
}