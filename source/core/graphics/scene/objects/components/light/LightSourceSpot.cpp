#include "LightSourceSpot.h"

using namespace Engine::Resources;
using namespace Engine::Core;
using namespace Engine::Core::Scene;

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