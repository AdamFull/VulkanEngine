#include "LightSourceDirectional.h"

using namespace engine::resources;
using namespace engine::core;
using namespace engine::core::scene;

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