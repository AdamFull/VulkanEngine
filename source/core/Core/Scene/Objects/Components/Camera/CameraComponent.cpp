#include "CameraComponent.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core::Scene::Objects::Components;

void CameraComponent::SetOrthographicProjection(float fLeft, float fRight, float fTop, float fBottom, float fNear, float fFar)
{
    projectionMatrix = glm::mat4{1.0f};
    projectionMatrix[0][0] = 2.f / (fRight - fLeft);
    projectionMatrix[1][1] = 2.f / (fBottom - fTop);
    projectionMatrix[2][2] = 1.f / (fFar - fNear);
    projectionMatrix[3][0] = -(fRight + fLeft) / (fRight - fLeft);
    projectionMatrix[3][1] = -(fBottom + fTop) / (fBottom - fTop);
    projectionMatrix[3][2] = -fNear / (fFar - fNear);
}

void CameraComponent::SetPerspectiveProjection(float fovy, float aspect, float fNear, float fFar)
{
    projectionMatrix = glm::perspective(glm::radians(fovy), aspect, fNear, fFar);
    if(bFlipY)
        projectionMatrix[1][1] *= -1.f;
    //viewMatrix[1][1] *= -1.f;
}

void CameraComponent::SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
{
    const glm::vec3 w{glm::normalize(direction)};
    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
    const glm::vec3 v{glm::cross(w, u)};

    viewMatrix = glm::mat4{1.f};
    viewMatrix[0][0] = u.x;
    viewMatrix[1][0] = u.y;
    viewMatrix[2][0] = u.z;
    viewMatrix[0][1] = v.x;
    viewMatrix[1][1] = v.y;
    viewMatrix[2][1] = v.z;
    viewMatrix[0][2] = w.x;
    viewMatrix[1][2] = w.y;
    viewMatrix[2][2] = w.z;
    viewMatrix[3][0] = -glm::dot(u, position);
    viewMatrix[3][1] = -glm::dot(v, position);
    viewMatrix[3][2] = -glm::dot(w, position);
}

void CameraComponent::SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
    SetViewDirection(position, target - position, up);
}

void CameraComponent::SetViewYXZ(glm::vec3 position, glm::vec3 rotation)
{
    glm::mat4 rotM = glm::mat4(1.0f);
	glm::mat4 transM;

	rotM = glm::rotate(rotM, glm::radians(rotation.x * (bFlipY ? -1.0f : 1.f)), glm::vec3(1.0f, 0.0f, 0.0f));
	rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 translation = position;
    if(bFlipY)
	    translation.y *= -1.0f;

	transM = glm::translate(glm::mat4(1.0f), translation);

	viewMatrix = rotM * transM;

    viewPos = glm::vec4(position, 0.0) * glm::vec4(-1.0, 1.0, -1.0, 1.0);
}

void CameraComponent::Update(float fDeltaTime)
{
    RenderObject::Update(fDeltaTime);

    auto aspect = USwapChain->GetAspectRatio();
    SetPerspectiveProjection(45.f, aspect, 0.01f, 1024.f);
    SetViewYXZ(m_transform.pos, m_transform.rot);
}