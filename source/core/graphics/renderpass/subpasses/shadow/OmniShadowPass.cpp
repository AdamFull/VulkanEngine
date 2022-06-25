#include "OmniShadowPass.h"
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

void COmniShadowPass::create()
{
    pMaterial = CMaterialLoader::inst()->create("omni_shadow_pass");
    pMaterial->addDefinition("INVOCATION_COUNT", std::to_string(MAX_POINT_LIGHT_COUNT));
    pMaterial->create();
    CSubpass::create();
}

void COmniShadowPass::reCreate()
{
    pMaterial->reCreate();
}

void COmniShadowPass::beforeRender(vk::CommandBuffer& commandBuffer)
{
    commandBuffer.setDepthBias(1.25f, 0.0f, 1.75f);
}

void COmniShadowPass::render(vk::CommandBuffer& commandBuffer)
{
    CRenderSystem::inst()->setStageType(EStageType::eShadow);
    CVBO::inst()->bind(commandBuffer);

    auto lightObjects = CLightSourceManager::inst()->getObjects();
    std::array<std::array<glm::mat4, 6>, MAX_POINT_LIGHT_COUNT> aPointViewProjMat;
    uint32_t light_count{0};
    for(auto& lightNode : lightObjects)
    {
        auto& light = lightNode->getLight();
        if(light && (light->getType() == ELightSourceType::ePoint))
            aPointViewProjMat[light_count] = light->getShadowViews();
    }

    auto& pUBOShadow = pMaterial->getUniformBuffer("UBOShadowmap");
    pUBOShadow->set("viewProjMat", aPointViewProjMat);
    pUBOShadow->set("passedLights", light_count);

    pMaterial->update();
    pMaterial->bind(commandBuffer);

    CSceneManager::inst()->getScene()->getRoot()->render(commandBuffer);
}

void COmniShadowPass::cleanup()
{
    CSubpass::cleanup();
}