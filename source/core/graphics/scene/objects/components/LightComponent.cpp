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

void CLightComponent::setColor(const glm::vec3& color)
{
    lightData.color = color;
}

const glm::vec3& CLightComponent::getColor()
{
    return lightData.color;
}

void CLightComponent::setInnerAngle(float angle)
{
    lightData.innerConeAngle = angle;
}

const float CLightComponent::getInnerAngle()
{
    return lightData.innerConeAngle;
}


void CLightComponent::setOuterAngle(float angle)
{
    lightData.outerConeAngle = angle;
}

const float CLightComponent::getOuterAngle()
{
    return lightData.outerConeAngle;
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
    lightData.position = transform.getPosition();
    lightData.direction = transform.getRotation();

    glm::mat4 shadowProj = glm::perspective(glm::radians(100.0f), 1.0f, 0.1f, 64.0f);
	glm::mat4 shadowView = glm::lookAt(glm::vec3(lightData.position), glm::vec3(lightData.direction), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 shadowModel = glm::mat4(1.0f);

    lightData.view = shadowProj * shadowView * shadowModel;

    return lightData;
}