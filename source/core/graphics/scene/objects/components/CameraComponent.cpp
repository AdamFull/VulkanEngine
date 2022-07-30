#include "CameraComponent.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"

using namespace engine::core::scene;

void CCameraComponent::create()
{
    auto& transform = pParent.lock()->getLocalTransform();
    //angleV = glm::degrees(glm::asin(-transform.rot.y));
    angleV = glm::degrees(-transform.rot.y);
    angleH = glm::degrees(glm::atan(transform.rot.z/transform.rot.x));
}

void CCameraComponent::update(float fDeltaTime)
{
    dt = fDeltaTime;
    auto transform = pParent.lock()->getLocalTransform();
    viewPos = glm::vec4(transform.pos, 0.0);

    auto view = getView();
    auto projection = getProjection();
    frustum.update(view, projection);
}

void CCameraComponent::moveForward(bool bInv)
{
    if(!bEnableControls)
        return;

    float dir = bInv ? -1.f : 1.f;
    auto& transform = pParent.lock()->getLocalTransform();
    transform.pos += getForwardVector() * dir * dt * sensitivity;
}

void CCameraComponent::moveRight(bool bInv)
{
    if(!bEnableControls)
        return;
        
    float dir = bInv ? -1.f : 1.f;
    auto& transform = pParent.lock()->getLocalTransform();
    transform.pos += getRightVector() * dir * dt * sensitivity;
}

void CCameraComponent::moveUp(bool bInv)
{
    if(!bEnableControls)
        return;
        
    float dir = bInv ? -1.f : 1.f;
    auto& transform = pParent.lock()->getLocalTransform();
    transform.pos += getUpVector() * dir * dt * sensitivity;
}

//TODO: make transform component as reference
void CCameraComponent::lookAt(float dX, float dY)
{
    if(!bEnableControls)
        return;
        
    float rotX = dX / dt;
    float rotY = dY / dt;

    angleH += rotX * sensitivity;
    if(angleV + rotY * sensitivity > 89)
        angleV = 89;
    else if(angleV + rotY * sensitivity < -89)
        angleV = -89;
    else
        angleV += rotY * sensitivity;

    const float w{cos(glm::radians(angleV)) * -cos(glm::radians(angleH))};
    const float u{-sin(glm::radians(angleV))};
    const float v{cos(glm::radians(angleV)) * -sin(glm::radians(angleH))};
    
    auto& transform = pParent.lock()->getLocalTransform();
    transform.rot = glm::normalize(glm::vec3(w * -1.f, u, v * -1.f));
}

glm::mat4 CCameraComponent::getProjection(bool flipY) const
{
    auto aspect = UDevice->getAspect(true);
    auto perspective = glm::perspective(glm::radians(fieldOfView), aspect, nearPlane, farPlane);
    if(flipY)
        perspective[1][1] *= -1.f;
    return perspective;
}

glm::mat4 CCameraComponent::getView(bool flipY) const
{
    auto& transform = pParent.lock()->getLocalTransform();

    auto position = transform.pos;
    if(flipY)
        position.y *= -1.f;

    return glm::lookAt(position, position + transform.rot, getUpVector());
}

glm::vec3 CCameraComponent::getForwardVector() const
{
    auto& transform = pParent.lock()->getLocalTransform();
    return transform.rot;
}

glm::vec3 CCameraComponent::getUpVector() const
{
    return glm::vec3{0.0, 1.0, 0.0};
}

glm::vec3 CCameraComponent::getRightVector() const
{
    return glm::normalize(glm::cross(getForwardVector(), getUpVector()));
}