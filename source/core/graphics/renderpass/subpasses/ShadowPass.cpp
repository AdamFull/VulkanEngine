#include "ShadowPass.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "resources/materials/MaterialShadow.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene::Objects;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

void CShadowPass::create(std::unique_ptr<FRenderCreateInfo>& createInfo)
{
    m_pMaterial = std::make_shared<MaterialShadow>();
    CSubpass::create(createInfo);
}

void CShadowPass::render(std::unique_ptr<FRenderProcessInfo>& renderData)
{
    auto imageIndex = USwapChain->GetCurrentFrame();
    UVBO->Bind(renderData->commandBuffer);
    m_pMaterial->Bind(renderData->commandBuffer, imageIndex);
    renderData->root->Render(renderData->commandBuffer, imageIndex);
}

void CShadowPass::cleanup()
{
    CSubpass::cleanup();
}