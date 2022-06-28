#include "CascadeShadowPass.h"
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

void CCascadeShadowPass::create()
{
    pMaterial = CMaterialLoader::inst()->create("directional_shadow_pass");
    pMaterial->addDefinition("INVOCATION_COUNT", std::to_string(SHADOW_MAP_CASCADE_COUNT));
    pMaterial->create();
    CSubpass::create();
}

void CCascadeShadowPass::reCreate()
{
    pMaterial->reCreate();
}

void CCascadeShadowPass::beforeRender(vk::CommandBuffer& commandBuffer)
{
    //commandBuffer.setDepthBias(1.25f, 0.0f, 1.75f);
}

void CCascadeShadowPass::render(vk::CommandBuffer& commandBuffer)
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
            aCascadeViewProjMat = light->getCascadeViews();
        }
    }

    auto& pUBOShadow = pMaterial->getUniformBuffer("UBOShadowmap");
    pUBOShadow->set("viewProjMat", aCascadeViewProjMat);
    pUBOShadow->set("passedLights", SHADOW_MAP_CASCADE_COUNT);

    pMaterial->update();
    pMaterial->bind(commandBuffer);

    CSceneManager::inst()->getScene()->getRoot()->render(commandBuffer);
    //commandBuffer.setDepthBias(1.0f, 0.0f, 1.0f);
}

void CCascadeShadowPass::cleanup()
{
    CSubpass::cleanup();
}