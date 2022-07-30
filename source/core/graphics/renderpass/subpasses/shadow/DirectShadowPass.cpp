#include "DirectShadowPass.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/SceneManager.h"
#include "graphics/scene/objects/components/LightSourceManager.h"
#include "GlobalVariables.h"

using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;
using namespace engine::resources::material;

void CDirectShadowPass::create()
{
    pMaterial = CMaterialLoader::inst()->create("directional_shadow_pass");
    pMaterial->addDefinition("INVOCATION_COUNT", std::to_string(MAX_SPOT_LIGHT_COUNT));
    pMaterial->create();
    CSubpass::create();
}

void CDirectShadowPass::reCreate()
{
    pMaterial->reCreate();
}

void CDirectShadowPass::beforeRender(vk::CommandBuffer& commandBuffer)
{
    //commandBuffer.setDepthBias(GlobalVariables::depthBiasConstant, 0.0f, GlobalVariables::depthBiasSlope);
}

void CDirectShadowPass::render(vk::CommandBuffer& commandBuffer)
{
    URenderer->setStageType(EStageType::eShadow);
    UVBO->bind(commandBuffer);

    auto lightObjects = ULightning->getObjects();
    std::array<glm::mat4, MAX_SPOT_LIGHT_COUNT> aSpotViewProjMat;
    uint32_t light_count{0};
    for(auto& lightNode : lightObjects)
    {
        auto light = lightNode.lock()->getComponent<CLightComponent>().lock();
        if(light && (light->getType() == ELightSourceType::eSpot))
        {
            aSpotViewProjMat[light_count] = light->getShadowView();
            light_count++;
        }
    }

    auto& pUBOShadow = pMaterial->getUniformBuffer("UBOShadowmap");
    pUBOShadow->set("viewProjMat", aSpotViewProjMat);
    pUBOShadow->set("passedLights", light_count);

    pMaterial->update();
    pMaterial->bind(commandBuffer);

    UScene->getScene()->getRoot()->render(commandBuffer);
}