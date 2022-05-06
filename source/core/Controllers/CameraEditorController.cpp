#include "CameraEditorController.h"
#include "keymapper/InputMapper.h"
#include "graphics/scene/objects/components/camera/CameraManager.h"
#include "util/Transform.hpp"

using namespace Engine::Controllers;
using namespace Engine::Core::Scene;

void CCameraEditorController::create()
{

    CInputMapper::inst()->createAction("CameraMovement", EActionKey::eW, EActionKey::eS, EActionKey::eA,
                                             EActionKey::eD, EActionKey::eSpace, EActionKey::eLeftControl, EActionKey::eMouseLeft);
    CInputMapper::inst()->createAction("CameraRotation", EActionKey::eCursorDelta);
    CInputMapper::inst()->createAction("CameraMovementToPoint", EActionKey::eScrol);

    CInputMapper::inst()->bindAction("CameraMovement", EKeyState::ePressed, this, &CCameraEditorController::cameraMovement);
    CInputMapper::inst()->bindAxis("CameraRotation", this, &CCameraEditorController::mouseRotation);
    CInputMapper::inst()->bindAxis("CameraMovementToPoint", this, &CCameraEditorController::cameraToPoint);
}

void CCameraEditorController::update(float fDeltaTime)
{
    CCameraController::update(fDeltaTime);
}

void CCameraEditorController::cameraMovement(EActionKey eKey, EKeyState eState)
{
    auto camera = CCameraManager::inst()->getCurrentCamera();
    FTransform transform = camera->getTransform();

    glm::vec3 direction{0.f};
    switch (eKey)
    {
    case EActionKey::eW:
        direction += transform.getForwardVector();
        break;
    case EActionKey::eS:
        direction -= transform.getForwardVector();
        break;
    case EActionKey::eA:
        direction -= transform.getRightVector();
        break;
    case EActionKey::eD:
        direction += transform.getRightVector();
        break;
    case EActionKey::eSpace:
        direction += transform.getUpVector();
        break;
    case EActionKey::eLeftControl:
        direction -= transform.getUpVector();
        break;
    case EActionKey::eMouseLeft:
        m_bRotatePass = true;
        break;

    default:
        break;
    }

    if (glm::dot(direction, direction) > std::numeric_limits<float>::epsilon())
    {
        transform.pos += m_fMoveSpeed * m_fDeltaTime * direction;
        camera->setTransform(transform);
    }
}

void CCameraEditorController::mouseRotation(float fX, float fY)
{
    if (!m_bRotatePass)
        return;

    auto camera = CCameraManager::inst()->getCurrentCamera();
    FTransform transform = camera->getTransform();
    glm::vec3 rotate = glm::vec3{fY * m_fLookSpeed * -1.f, fX * m_fLookSpeed, 0.f};

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
    {
        transform.rot += rotate * 20.0f;
        camera->setTransform(transform);
    }
    m_bRotatePass = false;
}

void CCameraEditorController::cameraToPoint(float fX, float fY)
{
    auto camera = CCameraManager::inst()->getCurrentCamera();
    FTransform transform = camera->getTransform();
    glm::vec3 direction = transform.getForwardVector() * fY;

    if (glm::dot(direction, direction) > std::numeric_limits<float>::epsilon())
    {
        transform.pos += m_fScrollSpeed * direction;
        camera->setTransform(transform);
    }
}