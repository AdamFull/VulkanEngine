#include "LightSourceDirectional.h"

using namespace Engine::Resources;
using namespace Engine::Core;
using namespace Engine::Core::Scene;

CLightSourceDirectional::CLightSourceDirectional()
{
    eLightType = ELightSourceType::eDirectional;
}

FDirectionalLight CLightSourceDirectional::getLight()
{
    //TODO: rotate vector direction
    FDirectionalLight light{};
    light.color = vColor;
    light.direction = transform.rot;
    light.intencity = fIntencity;
    return light;
}