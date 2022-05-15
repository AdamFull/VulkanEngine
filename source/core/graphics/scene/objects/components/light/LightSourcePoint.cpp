#include "LightSourcePoint.h"

using namespace Engine::Resources;
using namespace Engine::Core;
using namespace Engine::Core::Scene;

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