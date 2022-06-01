#include "LightSourceSpot.h"

using namespace engine::resources;
using namespace engine::core;
using namespace engine::core::scene;

CLightSourceSpot::CLightSourceSpot()
{
    eLightType = ELightSourceType::eSpot;
}

FSpotLight CLightSourceSpot::getLight()
{
    //TODO: rotate vector direction
    FSpotLight light{};
    light.position = getPosition();
    light.color = vColor;
    light.direction = transform.rot;
    light.intencity = fIntencity;
    light.cutoff = cutoff;
    return light;
}