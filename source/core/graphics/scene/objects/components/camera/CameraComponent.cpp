#include "CameraComponent.h"
#include "util/Frustum.hpp"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Util;
using namespace Engine::Core::Scene;

CCameraComponent::CCameraComponent()
{
    eObjectType = ERenderObjectType::eCamera;
}

void CCameraComponent::update(float fDeltaTime)
{
    CRenderObject::update(fDeltaTime); 
    dt = fDeltaTime;
    viewPos = glm::vec4(transform.pos, 0.0);
    transform.rot = direction;
    CFrustum::inst()->update(getView(), getProjection());
    //TODO: check frustum in camera
}

void CCameraComponent::moveForward(bool bInv)
{
    if(!bEnableControls)
        return;

    float dir = bInv ? -1.f : 1.f;
    transform.pos += getForwardVector() * dir * dt * sensitivity;
}

void CCameraComponent::moveRight(bool bInv)
{
    if(!bEnableControls)
        return;
        
    float dir = bInv ? -1.f : 1.f;
    transform.pos += getRightVector() * dir * dt * sensitivity;
}

void CCameraComponent::moveUp(bool bInv)
{
    if(!bEnableControls)
        return;
        
    float dir = bInv ? -1.f : 1.f;
    transform.pos += getUpVector() * dir * dt * sensitivity;
}

void CCameraComponent::lookAt(float dX, float dY)
{
    if(!bEnableControls)
        return;
        
    float rotX = dX/2;
    float rotY = dY/2;
    float sens = 100.f * sensitivity * 1.f / dt;

    angleH += rotX*sens;
    if(angleV + rotY*sens > 89)
        angleV = 89;
    else if(angleV + rotY*sens < -89)
        angleV = -89;
    else
        angleV += rotY*sens;

    const float w{cos(glm::radians(angleV)) * -cos(glm::radians(angleH))};
    const float u{-sin(glm::radians(angleV))};
    const float v{cos(glm::radians(angleV)) * -sin(glm::radians(angleH))};
    
    direction = glm::normalize(glm::vec3(w, u, v));
}

glm::mat4 CCameraComponent::getProjection() const
{
    auto aspect = CDevice::inst()->getAspect(true);
    auto perspective = glm::perspective(glm::radians(fieldOfView), aspect, near, far);
    perspective[1][1] *= -1.f;
    return perspective;
}

glm::mat4 CCameraComponent::getView() const
{
    return glm::lookAt(transform.pos, transform.pos + direction, getUpVector());
}

glm::vec3 CCameraComponent::getForwardVector() const
{
    return direction;
}

glm::vec3 CCameraComponent::getUpVector() const
{
    return glm::vec3{0.0, 1.0, 0.0};
}

glm::vec3 CCameraComponent::getRightVector() const
{
    return glm::normalize(glm::cross(getForwardVector(), getUpVector()));
}
