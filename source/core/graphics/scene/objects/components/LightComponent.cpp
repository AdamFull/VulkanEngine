#include "LightComponent.h"
#include "graphics/scene/objects/RenderObject.h"

using namespace engine::core::scene;
using namespace engine::resources;

void CLightComponent::setType(ELightSourceType type)
{
    lightData.type = static_cast<int32_t>(type);
}

ELightSourceType CLightComponent::getType()
{
    return static_cast<ELightSourceType>(lightData.type);
}

void CLightComponent::setRadius(float radius)
{
    lightData.radius = radius;
}

const float CLightComponent::getRadius()
{
    return lightData.radius;
}

void CLightComponent::setColor(const glm::vec3& color)
{
    lightData.color = color;
}

const glm::vec3& CLightComponent::getColor()
{
    return lightData.color;
}

void CLightComponent::setIntencity(float intencity)
{
    lightData.intencity = intencity;
}

const float CLightComponent::getIntencity()
{
    return lightData.intencity;
}



FLight& CLightComponent::getLight()
{
    auto transform = pParent->getTransform();
    lightData.position = transform.pos;
    lightData.direction = transform.rot;
    return lightData;
}