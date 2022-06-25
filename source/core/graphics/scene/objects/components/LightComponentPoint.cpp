#include "LightComponentPoint.h"

using namespace engine::core::scene;
using namespace engine::resources;

void CLightComponentPoint::create()
{
    CLightComponent::create();
}

void CLightComponentPoint::update(float fDeltaTime)
{
    CLightComponent::update(fDeltaTime);


    glm::mat4 shadowProj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 64.f);
    for(uint32_t face = 0; face < 6; face++)
    {
        glm::mat4 shadowView = glm::mat4(1.0f);
        switch (face)
        {
        case 0: // POSITIVE_X
            shadowView = glm::rotate(shadowView, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            shadowView = glm::rotate(shadowView, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case 1:	// NEGATIVE_X
            shadowView = glm::rotate(shadowView, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            shadowView = glm::rotate(shadowView, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case 2:	// POSITIVE_Y
            shadowView = glm::rotate(shadowView, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case 3:	// NEGATIVE_Y
            shadowView = glm::rotate(shadowView, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case 4:	// POSITIVE_Z
            shadowView = glm::rotate(shadowView, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case 5:	// NEGATIVE_Z
            shadowView = glm::rotate(shadowView, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            break;
        }

        this->shadowViews[face] = shadowProj * shadowView;
    }
}
