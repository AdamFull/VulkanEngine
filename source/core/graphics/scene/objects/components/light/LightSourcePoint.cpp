#include "LightSourcePoint.h"

using namespace engine::resources;
using namespace engine::core;
using namespace engine::core::scene;

CLightSourcePoint::CLightSourcePoint()
{
    eLightType = ELightSourceType::ePoint;
}

FPointLight CLightSourcePoint::getLight()
{
    FPointLight light{};
    light.position = getPosition();
    light.color = vColor;
    light.radius = fAttenuation;
    light.intencity = fIntencity;
    return light;
}