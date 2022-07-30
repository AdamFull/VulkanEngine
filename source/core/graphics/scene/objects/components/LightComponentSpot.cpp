#include "LightComponentSpot.h"
#include "graphics/scene/objects/RenderObject.h"

using namespace engine::core::scene;
using namespace engine::resources;

void CLightComponentSpot::create()
{
    CLightComponent::create();
}

void CLightComponentSpot::update(float fDeltaTime)
{
    CLightComponent::update(fDeltaTime);

    const glm::vec3 dp(0.0000001f);
    auto transform = pParent.lock()->getTransform();
    glm::mat4 shadowProj = glm::perspective(this->innerConeAngle, 1.0f, 0.1f, 64.f);
	glm::mat4 shadowView = glm::lookAt(transform.pos + dp, transform.rot, glm::vec3(0.0f, 1.0f, 0.0f));

    this->shadowView = shadowProj * shadowView;
}
