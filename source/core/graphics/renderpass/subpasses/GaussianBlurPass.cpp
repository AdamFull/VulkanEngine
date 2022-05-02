#include "GaussianBlurPass.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "resources/materials/MaterialBlur.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "GlobalVariables.h"

using namespace Engine::Core::Render;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

void CGaussianBlurPass::create(std::shared_ptr<Resources::CResourceManager>& resourceManager, std::shared_ptr<Scene::CRenderObject>& root, vk::RenderPass& renderPass, uint32_t subpass)
{
    auto framesInFlight = USwapChain->getFramesInFlight();
    pUniform = std::make_shared<CUniformBuffer>();
    pUniform->create(framesInFlight, sizeof(FBlurData));

    pMaterial = std::make_shared<CMaterialBlur>();
    pMaterial->create(renderPass, subpass);
    CSubpass::create(resourceManager, root, renderPass, subpass);
}

void CGaussianBlurPass::render(vk::CommandBuffer& commandBuffer, std::unordered_map<std::string, std::shared_ptr<CImage>>& images, std::shared_ptr<Scene::CRenderObject>& root)
{
    auto imageIndex = USwapChain->getCurrentFrame();
    FBlurData uniform;
    uniform.blurScale = GlobalVariables::blurScale;
    uniform.blurStrength = GlobalVariables::blurStrength;
    uniform.direction = direction;

    pUniform->updateUniformBuffer(imageIndex, &uniform);
    auto& buffer = pUniform->getUniformBuffer(imageIndex);
    auto descriptor = buffer->getDscriptor();
    pMaterial->update(descriptor, imageIndex);
    pMaterial->bind(commandBuffer, imageIndex);
    commandBuffer.draw(3, 1, 0, 0);
}

void CGaussianBlurPass::cleanup()
{
    CSubpass::cleanup();
}