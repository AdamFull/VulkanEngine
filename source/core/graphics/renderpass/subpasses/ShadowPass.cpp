#include "ShadowPass.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/SceneManager.h"
#include "graphics/scene/objects/components/LightSourceManager.h"

using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;
using namespace engine::resources::material;

void CShadowPass::create()
{
    pMaterial = CMaterialLoader::inst()->create("directional_shadow_pass");
    pMaterial->addDefinition("CASCADES_COUNT", std::to_string(SHADOW_MAP_CASCADE_COUNT));
    pMaterial->create();
    CSubpass::create();

    //Cascade shadows for directional light
    //Simple shadows for spot lights
    //Omni shadows for point lights
}

void CShadowPass::reCreate()
{
    pMaterial->reCreate();
}

void CShadowPass::beforeRender(vk::CommandBuffer& commandBuffer)
{
    //commandBuffer.setDepthBias(1.25f, 0.0f, 1.75f);
}

void CShadowPass::render(vk::CommandBuffer& commandBuffer)
{
    CRenderSystem::inst()->setStageType(EStageType::eShadow);
    CVBO::inst()->bind(commandBuffer);

    auto lightObjects = CLightSourceManager::inst()->getObjects();
    std::array<glm::mat4, SHADOW_MAP_CASCADE_COUNT> aCascadeViewProjMat;
    for(auto& lightNode : lightObjects)
    {
        auto& light = lightNode->getLight();
        if(light && (light->getType() == ELightSourceType::eDirectional))
        {
            auto& lightData = light->getLight();
            aCascadeViewProjMat = lightData.aCascadeViewProjMat;
        }
    }

    auto& pUBOShadow = pMaterial->getUniformBuffer("UBOShadowmap");
    pUBOShadow->set("cascadeViewProjMat", aCascadeViewProjMat);

    pMaterial->update();
    pMaterial->bind(commandBuffer);

    CSceneManager::inst()->getScene()->getRoot()->render(commandBuffer);
    //commandBuffer.setDepthBias(1.0f, 0.0f, 1.0f);
}

void CShadowPass::cleanup()
{
    CSubpass::cleanup();
}