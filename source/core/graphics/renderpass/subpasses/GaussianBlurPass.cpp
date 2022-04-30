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

void CGaussianBlurPass::create(std::shared_ptr<ResourceManager> resourceManager, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<RenderObject> root, vk::RenderPass& renderPass, uint32_t subpass)
{
    auto framesInFlight = USwapChain->GetFramesInFlight();
    m_pUniform = std::make_shared<UniformBuffer>();
    m_pUniform->Create(framesInFlight, sizeof(FBlurData));

    m_pMaterial = std::make_shared<MaterialBlur>();
    m_pMaterial->Create(renderPass, subpass);
}

void CGaussianBlurPass::render(vk::CommandBuffer& commandBuffer, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<RenderObject> root)
{
    auto imageIndex = USwapChain->GetCurrentFrame();
    FBlurData uniform;
    uniform.blurScale = GlobalVariables::blurScale;
    uniform.blurStrength = GlobalVariables::blurStrength;
    uniform.direction = direction;

    m_pUniform->UpdateUniformBuffer(imageIndex, &uniform);
    auto& buffer = m_pUniform->GetUniformBuffer(imageIndex);
    auto descriptor = buffer->GetDscriptor();
    m_pMaterial->Update(descriptor, imageIndex);
    m_pMaterial->Bind(commandBuffer, imageIndex);
    commandBuffer.draw(3, 1, 0, 0);
}

void CGaussianBlurPass::cleanup()
{
    CSubpass::cleanup();
}