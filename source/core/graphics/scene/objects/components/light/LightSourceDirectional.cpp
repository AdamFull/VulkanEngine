#include "LightSourceDirectional.h"

using namespace Engine::Resources;
using namespace Engine::Core;
using namespace Engine::Core::Scene;

FDirectionalLight CLightSourceDirectional::getLight()
{
    //TODO: rotate vector direction
    FDirectionalLight light{};
    light.position = getPosition();
    light.color = vColor;
    light.direction = transform.rot;
    light.intencity = fIntencity;
    return light;
}