#include "SSRPass.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/scene/objects/components/CameraManager.h"

using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;
using namespace engine::resources::material;

void CSSRPass::create()
{
    pMaterial = CMaterialLoader::inst()->create("ssr_pass");
    pMaterial->create();

    CSubpass::create();
}

void CSSRPass::reCreate()
{
    CSubpass::reCreate();
}

void CSSRPass::render(vk::CommandBuffer &commandBuffer)
{
    URenderer->setStageType(EStageType::eDeferred);
    auto& images = URenderer->getCurrentImages();

    pMaterial->addTexture("packed_tex", images["packed_tex"]);
    pMaterial->addTexture("depth_tex", images["depth_tex"]);

    auto& cameraNode = UCamera->getCurrentCamera();
    auto& camera = cameraNode->getCamera();
    auto view = camera->getView();
    auto projection = camera->getProjection();
    auto viewProjection = projection * view;
    auto invView = glm::inverse(view);
    auto invProjection = glm::inverse(projection);
    auto invViewProjection = glm::inverse(viewProjection);

    auto& pUBO = pMaterial->getUniformBuffer("UBOReflections");
    pUBO->set("invViewProjection", invViewProjection);
    pUBO->set("viewProjection", viewProjection);
    pUBO->set("projection", projection);
    pUBO->set("invView", invView);
    pUBO->set("camPos", camera->viewPos);

    pMaterial->update();
    pMaterial->bind(commandBuffer);

    commandBuffer.draw(3, 1, 0, 0);
}