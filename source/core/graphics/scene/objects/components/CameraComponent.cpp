#include "CameraComponent.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"

using namespace engine::core::scene;

void CCameraComponent::create()
{
    auto transform = pParent->getLocalTransform();
    transform.rot = glm::vec3{-1.0, 0.0, 0.0};
    pParent->setTransform(transform);
}

void CCameraComponent::update(float fDeltaTime)
{
    dt = fDeltaTime;
    auto transform = pParent->getTransform();
    viewPos = glm::vec4(transform.pos, 0.0);

    auto view = getView();
    auto projection = getProjection();
    std::array<float, 16> clip;
    clip[0] = view[0][0] * projection[0][0] + view[0][1] * projection[1][0] + view[0][2] * projection[2][0] + view[0][3] * projection[3][0];
	clip[1] = view[0][0] * projection[0][1] + view[0][1] * projection[1][1] + view[0][2] * projection[2][1] + view[0][3] * projection[3][1];
	clip[2] = view[0][0] * projection[0][2] + view[0][1] * projection[1][2] + view[0][2] * projection[2][2] + view[0][3] * projection[3][2];
	clip[3] = view[0][0] * projection[0][3] + view[0][1] * projection[1][3] + view[0][2] * projection[2][3] + view[0][3] * projection[3][3];

    clip[4] = view[1][0] * projection[0][0] + view[1][1] * projection[1][0] + view[1][2] * projection[2][0] + view[1][3] * projection[3][0];
	clip[5] = view[1][0] * projection[0][1] + view[1][1] * projection[1][1] + view[1][2] * projection[2][1] + view[1][3] * projection[3][1];
	clip[6] = view[1][0] * projection[0][2] + view[1][1] * projection[1][2] + view[1][2] * projection[2][2] + view[1][3] * projection[3][2];
	clip[7] = view[1][0] * projection[0][3] + view[1][1] * projection[1][3] + view[1][2] * projection[2][3] + view[1][3] * projection[3][3];

    clip[8] = view[2][0] * projection[0][0] + view[2][1] * projection[1][0] + view[2][2] * projection[2][0] + view[2][3] * projection[3][0];
	clip[9] = view[2][0] * projection[0][1] + view[2][1] * projection[1][1] + view[2][2] * projection[2][1] + view[2][3] * projection[3][1];
	clip[10] = view[2][0] * projection[0][2] + view[2][1] * projection[1][2] + view[2][2] * projection[2][2] + view[2][3] * projection[3][2];
	clip[11] = view[2][0] * projection[0][3] + view[2][1] * projection[1][3] + view[2][2] * projection[2][3] + view[2][3] * projection[3][3];

    clip[12] = view[3][0] * projection[0][0] + view[3][1] * projection[1][0] + view[3][2] * projection[2][0] + view[3][3] * projection[3][0];
	clip[13] = view[3][0] * projection[0][1] + view[3][1] * projection[1][1] + view[3][2] * projection[2][1] + view[3][3] * projection[3][1];
	clip[14] = view[3][0] * projection[0][2] + view[3][1] * projection[1][2] + view[3][2] * projection[2][2] + view[3][3] * projection[3][2];
	clip[15] = view[3][0] * projection[0][3] + view[3][1] * projection[1][3] + view[3][2] * projection[2][3] + view[3][3] * projection[3][3];

    //Left
    frustumSides[1][0] = clip[3] - clip[0];
	frustumSides[1][1] = clip[7] - clip[4];
	frustumSides[1][2] = clip[11] - clip[8];
	frustumSides[1][3] = clip[15] - clip[12];
    normalizeFrustumSide(1);

    //Right
    frustumSides[0][0] = clip[3] + clip[0];
	frustumSides[0][1] = clip[7] + clip[4];
	frustumSides[0][2] = clip[11] + clip[8];
	frustumSides[0][3] = clip[15] + clip[12];
	normalizeFrustumSide(0);

    //Top
    frustumSides[3][0] = clip[3] - clip[1];
	frustumSides[3][1] = clip[7] - clip[5];
	frustumSides[3][2] = clip[11] - clip[9];
	frustumSides[3][3] = clip[15] - clip[13];
	normalizeFrustumSide(3);

    //Bottom
    frustumSides[2][0] = clip[3] + clip[1];
	frustumSides[2][1] = clip[7] + clip[5];
	frustumSides[2][2] = clip[11] + clip[9];
	frustumSides[2][3] = clip[15] + clip[13];
	normalizeFrustumSide(2);

    //Back
    frustumSides[4][0] = clip[3] + clip[2];
	frustumSides[4][1] = clip[7] + clip[6];
	frustumSides[4][2] = clip[11] + clip[10];
	frustumSides[4][3] = clip[15] + clip[14];
	normalizeFrustumSide(4);

    //Front
    frustumSides[5][0] = clip[3] - clip[2];
	frustumSides[5][1] = clip[7] - clip[6];
	frustumSides[5][2] = clip[11] - clip[10];
	frustumSides[5][3] = clip[15] - clip[14];
}

void CCameraComponent::moveForward(bool bInv)
{
    if(!bEnableControls)
        return;

    float dir = bInv ? -1.f : 1.f;
    auto transform = pParent->getLocalTransform();
    transform.pos += getForwardVector() * dir * dt * sensitivity;
    pParent->setTransform(transform);
}

void CCameraComponent::moveRight(bool bInv)
{
    if(!bEnableControls)
        return;
        
    float dir = bInv ? -1.f : 1.f;
    auto transform = pParent->getLocalTransform();
    transform.pos += getRightVector() * dir * dt * sensitivity;
    pParent->setTransform(transform);
}

void CCameraComponent::moveUp(bool bInv)
{
    if(!bEnableControls)
        return;
        
    float dir = bInv ? -1.f : 1.f;
    auto transform = pParent->getLocalTransform();
    transform.pos += getUpVector() * dir * dt * sensitivity;
    pParent->setTransform(transform);
}

//TODO: make transform component as reference
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
    
    auto transform = pParent->getLocalTransform();
    transform.rot = glm::normalize(glm::vec3(w, u, v));
    pParent->setTransform(transform);
}

glm::mat4 CCameraComponent::getProjection() const
{
    auto aspect = CDevice::inst()->getAspect(true);
    auto perspective = glm::perspective(glm::radians(fieldOfView), aspect, nearPlane, farPlane);
    return perspective;
}

glm::mat4 CCameraComponent::getView() const
{
    auto transform = pParent->getLocalTransform();
    return glm::lookAt(transform.pos, transform.pos + transform.rot, getUpVector());
}

glm::vec3 CCameraComponent::getForwardVector() const
{
    auto transform = pParent->getLocalTransform();
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

bool CCameraComponent::checkPoint(const glm::vec3& pos)
{
    for (uint32_t i = 0; i < 6; i++) 
    {
		if (frustumSides[i][0] * pos.x + frustumSides[i][1] * pos.y + frustumSides[i][2] * pos.z + frustumSides[i][3] <= 0.0f)
			return false;
	}
	return true;
}

bool CCameraComponent::checkSphere(const glm::vec3& pos, float radius)
{
    for (uint32_t i = 0; i < 6; i++) 
    {
		if (frustumSides[i][0] * pos.x + frustumSides[i][1] * pos.y + frustumSides[i][2] * pos.z + frustumSides[i][3] <= -radius) 
			return false;
	}
	return true;
}

bool CCameraComponent::checkBox(const glm::vec3& start, const glm::vec3& end)
{
    for (uint32_t i = 0; i < 6; i++) 
    {
		if (   frustumSides[i][0] * start.x + frustumSides[i][1] * start.y + frustumSides[i][2] * start.z + frustumSides[i][3] <= 0.0f
			&& frustumSides[i][0] * end.x + frustumSides[i][1] * start.y + frustumSides[i][2] * start.z + frustumSides[i][3] <= 0.0f
			&& frustumSides[i][0] * start.x + frustumSides[i][1] * end.y + frustumSides[i][2] * start.z + frustumSides[i][3] <= 0.0f
			&& frustumSides[i][0] * end.x + frustumSides[i][1] * end.y + frustumSides[i][2] * start.z + frustumSides[i][3] <= 0.0f
			&& frustumSides[i][0] * start.x + frustumSides[i][1] * start.y + frustumSides[i][2] * end.z + frustumSides[i][3] <= 0.0f
			&& frustumSides[i][0] * end.x + frustumSides[i][1] * start.y + frustumSides[i][2] * end.z + frustumSides[i][3] <= 0.0f
			&& frustumSides[i][0] * start.x + frustumSides[i][1] * end.y + frustumSides[i][2] * end.z + frustumSides[i][3] <= 0.0f
			&& frustumSides[i][0] * end.x + frustumSides[i][1] * end.y + frustumSides[i][2] * end.z + frustumSides[i][3] <= 0.0f)
			return false;
	}
	return true;
}

void CCameraComponent::normalizeFrustumSide(size_t side)
{
    auto magnitude = std::sqrt(frustumSides[side][0] * frustumSides[side][0] + frustumSides[side][1] * frustumSides[side][1] + frustumSides[side][2] * frustumSides[side][2]);
	frustumSides[side][0] /= magnitude;
	frustumSides[side][1] /= magnitude;
	frustumSides[side][2] /= magnitude;
	frustumSides[side][3] /= magnitude;
}