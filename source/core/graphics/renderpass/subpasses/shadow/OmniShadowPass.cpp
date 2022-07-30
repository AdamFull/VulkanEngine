#include "OmniShadowPass.h"
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
    //commandBuffer.setDepthBias(GlobalVariables::depthBiasConstant, 0.0f, GlobalVariables::depthBiasSlope);
}

void COmniShadowPass::render(vk::CommandBuffer& commandBuffer)
{
    URenderer->setStageType(EStageType::eShadow);
    UVBO->bind(commandBuffer);

    auto lightObjects = ULightning->getObjects();
    
    uint32_t light_count{0};
    std::array<glm::mat4, MAX_POINT_LIGHT_COUNT * 6> aPointViewProjMat;
    std::array<glm::vec4, MAX_POINT_LIGHT_COUNT> aLightPos;
    for(auto& lightNode : lightObjects)
    {
        uint32_t array_shift = light_count * 6;
        auto light = lightNode.lock()->getComponent<CLightComponent>().lock();
        if(light && (light->getType() == ELightSourceType::ePoint))
        {
            for(uint32_t face = 0; face < 6; face++)
            {
                auto& shadowViews = light->getShadowViews();
                aPointViewProjMat[face + array_shift] = shadowViews[face];
            }
                
            aLightPos[light_count] = glm::vec4(light->getPosition(), 1.0);
            light_count++;
        }
    }

    auto& pUBOShadow = pMaterial->getUniformBuffer("UBOShadowmap");
    pUBOShadow->set("viewProjMat", aPointViewProjMat);
    pUBOShadow->set("lightPos", aLightPos);
    pUBOShadow->set("passedLights", light_count);
    pUBOShadow->set("farPlane", 25.f);

    pMaterial->update();
    pMaterial->bind(commandBuffer);

    UScene->getScene()->getRoot()->render(commandBuffer);
}