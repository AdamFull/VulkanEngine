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
    pMaterial->create();
    CSubpass::create();
}

void CShadowPass::reCreate()
{
    pMaterial->reCreate();
}

void CShadowPass::render(vk::CommandBuffer& commandBuffer)
{
    CRenderSystem::inst()->setStageType(EStageType::eShadow);
    CVBO::inst()->bind(commandBuffer);

    uint32_t light_count{0};
    auto lightObjects = CLightSourceManager::inst()->getObjects();
    for(auto& lightNode : lightObjects)
    {
        auto& light = lightNode->getLight();
        if(light && (light->getType() == ELightSourceType::eDirectional || light->getType() == ELightSourceType::eSpot))
        {
            auto& lightData = light->getLight();
            aLightProjections[light_count] = lightData.view;
            light_count++;
        }
    }

    auto& pUBOShadow = pMaterial->getUniformBuffer("UBOShadowmap");
    pUBOShadow->set("mvp", aLightProjections);
    pUBOShadow->set("light_count", light_count);

    pMaterial->update();
    pMaterial->bind(commandBuffer);

    commandBuffer.setDepthBias(1.25f, 0.0f, 1.75f);
    CSceneManager::inst()->getScene()->getRoot()->render(commandBuffer);
    commandBuffer.setDepthBias(1.0f, 0.0f, 1.0f);
}

void CShadowPass::cleanup()
{
    CSubpass::cleanup();
}