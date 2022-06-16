#include "ScreenSpaceReflectionsPass.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/scene/objects/components/CameraManager.h"

using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;
using namespace engine::resources::material;

void CScreenSpaceReflectionsPass::create()
{
    pMaterial = CMaterialLoader::inst()->create("screen_space_reflections");
    pMaterial->create();
    CSubpass::create();
}

void CScreenSpaceReflectionsPass::reCreate()
{
    pMaterial->reCreate();
}

void CScreenSpaceReflectionsPass::render(vk::CommandBuffer& commandBuffer)
{
    auto& images = CRenderSystem::inst()->getCurrentImages();
    pMaterial->addTexture("color_tex", images["composition_tex"]);
    pMaterial->addTexture("packed_tex", images["packed_tex"]);
    pMaterial->addTexture("depth_tex", images["depth_image"]);

    auto& cameraNode = CCameraManager::inst()->getCurrentCamera();
    auto& camera = cameraNode->getCamera();
    auto view = camera->getView();
    auto projection = camera->getProjection();
    auto invViewProjection = glm::inverse(projection * view);

    auto& pUBO = pMaterial->getPushConstant("ubo");
    pUBO->set("invViewProjection", invViewProjection);
    pUBO->set("projection", projection);
    pUBO->set("viewPos", camera->viewPos);
    pUBO->flush(commandBuffer);

    pMaterial->update();
    pMaterial->bind(commandBuffer);

    commandBuffer.draw(3, 1, 0, 0);
}

void CScreenSpaceReflectionsPass::cleanup()
{
    CSubpass::cleanup();
}