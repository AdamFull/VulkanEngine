#include "GaussianBlurPass.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "resources/materials/MaterialBlur.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "GlobalVariables.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene::Objects;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

void CGaussianBlurPass::create(std::unique_ptr<FRenderCreateInfo>& createInfo)
{
    auto framesInFlight = USwapChain->GetFramesInFlight();
    pUniform = std::make_shared<CUniformBuffer>();
    pUniform->create(framesInFlight, sizeof(FBlurData));

    pMaterial = std::make_shared<CMaterialBlur>();
    pMaterial->create(createInfo->renderPass, createInfo->subpass);
    CSubpass::create(createInfo);
}

void CGaussianBlurPass::render(std::unique_ptr<FRenderProcessInfo>& renderData)
{
    auto imageIndex = USwapChain->GetCurrentFrame();
    FBlurData uniform;
    uniform.blurScale = GlobalVariables::blurScale;
    uniform.blurStrength = GlobalVariables::blurStrength;
    uniform.direction = direction;

    pUniform->updateUniformBuffer(imageIndex, &uniform);
    auto& buffer = pUniform->getUniformBuffer(imageIndex);
    auto descriptor = buffer->getDscriptor();
    pMaterial->update(descriptor, imageIndex);
    pMaterial->bind(renderData->commandBuffer, imageIndex);
    renderData->commandBuffer.draw(3, 1, 0, 0);
}

void CGaussianBlurPass::cleanup()
{
    CSubpass::cleanup();
}