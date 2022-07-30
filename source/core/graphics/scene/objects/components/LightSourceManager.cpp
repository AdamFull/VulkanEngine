#include "LightSourceManager.h"
#include "graphics/VulkanHighLevel.h"

//TODO: move managers to another space

using namespace engine::core;
using namespace engine::core::scene;

void CLightSourceManager::addLight(utl::ref_ptr<CRenderObject>& light)
{
    vLights.emplace_back(light);
}

directional_lights_array& CLightSourceManager::getDirectionalSources(uint32_t& light_count)
{
    light_count = 0;
    for(auto& light : vLights)
    {
        auto lightSource = light.lock()->getComponent<CLightComponent>().lock();
        if(lightSource->getType() == resources::ELightSourceType::eDirectional)
        {
            auto& light_data = directionalLights[light_count];
            light_data.direction = lightSource->getDirection();
            light_data.color = lightSource->getColor();
            light_data.intencity = lightSource->getIntencity();
            light_data.cascadeSplits = lightSource->getCascadeSplits();
            light_data.cascadeViewProjMat = lightSource->getCascadeViews();
            light_count++;
        }
    }
    return directionalLights;
}

point_lights_array& CLightSourceManager::getPointSources(uint32_t& light_count)
{
    light_count = 0;
    for(auto& light : vLights)
    {
        auto lightSource = light.lock()->getComponent<CLightComponent>().lock();
        if(lightSource->getType() == resources::ELightSourceType::ePoint)
        {
            auto& light_data = pointLights[light_count];
            light_data.position = lightSource->getPosition();
            light_data.color = lightSource->getColor();
            light_data.intencity = lightSource->getIntencity();
            light_data.radius = lightSource->getRadius();
            light_count++;
        }
    }
    return pointLights;
}

spot_lights_array& CLightSourceManager::getSpotSources(uint32_t& light_count)
{
    light_count = 0;
    for(auto& light : vLights)
    {
        auto lightSource = light.lock()->getComponent<CLightComponent>().lock();
        if(lightSource->getType() == resources::ELightSourceType::eSpot)
        {
            auto& light_data = spotLights[light_count];
            light_data.shadowView = lightSource->getShadowView();
            light_data.direction = lightSource->getDirection();
            light_data.position = lightSource->getPosition();
            light_data.color = lightSource->getColor();
            light_data.intencity = lightSource->getIntencity();
            light_data.innerConeAngle = lightSource->getInnerAngle();
            light_data.outerConeAngle = lightSource->getOuterAngle();
            light_count++;
        }
    }
    return spotLights;
}