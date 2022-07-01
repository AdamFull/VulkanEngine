#include "LightComponentPoint.h"
#include "graphics/scene/objects/RenderObject.h"

using namespace engine::core::scene;
using namespace engine::resources;

void CLightComponentPoint::create()
{
    CLightComponent::create();
}

void CLightComponentPoint::update(float fDeltaTime)
{
    CLightComponent::update(fDeltaTime);

    auto& transform = pParent->getLocalTransform();
	//float angle = glm::radians(componentTick * 360.0f);
	//float radius = -5.0f;
	//transform.pos = glm::vec3(cos(angle) * radius, -radius, sin(angle) * radius);

    //Based on https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 32.f);
    auto position = transform.pos;// * glm::vec3(1.0, -1.0, 1.0);
    this->shadowViews[0] = shadowProj * glm::lookAt(position, position + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)); // POSITIVE_X
    this->shadowViews[1] = shadowProj * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)); // NEGATIVE_X
    this->shadowViews[2] = shadowProj * glm::lookAt(position, position + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)); // POSITIVE_Y
    this->shadowViews[3] = shadowProj * glm::lookAt(position, position + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0));	// NEGATIVE_Y
    this->shadowViews[4] = shadowProj * glm::lookAt(position, position + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0));	// POSITIVE_Z
    this->shadowViews[5] = shadowProj * glm::lookAt(position, position + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0));	// NEGATIVE_Z
}
