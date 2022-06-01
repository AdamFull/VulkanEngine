#include "CameraEditorController.h"
#include "keymapper/InputMapper.h"
#include "graphics/scene/objects/components/camera/CameraManager.h"
#include "util/Transform.hpp"

using namespace engine::controllers;
using namespace engine::core::scene;

void CCameraEditorController::create()
{

    CInputMapper::inst()->createAction("CameraMovement", EActionKey::eW, EActionKey::eS, EActionKey::eA,
                                             EActionKey::eD, EActionKey::eSpace, EActionKey::eLeftControl, EActionKey::eMouseMiddle);
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

    switch (eKey)
    {
    case EActionKey::eW:
        camera->moveForward(false);
        break;
    case EActionKey::eS:
        camera->moveForward(true);
        break;
    case EActionKey::eA:
        camera->moveRight(true);
        break;
    case EActionKey::eD:
        camera->moveRight(false);
        break;
    case EActionKey::eSpace:
        camera->moveUp(false);
        break;
    case EActionKey::eLeftControl:
        camera->moveUp(true);
        break;
    case EActionKey::eMouseMiddle:
        m_bRotatePass = true;
        break;

    default:
        break;
    }
}

void CCameraEditorController::mouseRotation(float fX, float fY)
{
    if (!m_bRotatePass)
        return;

    auto camera = CCameraManager::inst()->getCurrentCamera();
    camera->lookAt(fX, fY);
    m_bRotatePass = false;
}

void CCameraEditorController::cameraToPoint(float fX, float fY)
{
    /*auto camera = CCameraManager::inst()->getCurrentCamera();
    FTransform transform = camera->getTransform();
    glm::vec3 direction = transform.getForwardVector() * fY;

    if (glm::dot(direction, direction) > std::numeric_limits<float>::epsilon())
    {
        camera->moveForward(false);
    }*/
}