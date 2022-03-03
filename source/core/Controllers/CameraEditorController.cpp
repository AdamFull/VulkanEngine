#include "CameraEditorController.h"
#include "KeyMapping/InputMapper.h"
#include "Core/Scene/Objects/Components/Camera/CameraManager.h"

using namespace Engine::Controllers;
using namespace Engine::Core::Scene::Objects;

void CameraEditorController::Create()
{

    InputMapper::getInstance()->CreateAction("CameraMovement", EActionKey::eW, EActionKey::eS, EActionKey::eA,
                                             EActionKey::eD, EActionKey::eSpace, EActionKey::eLeftControl, EActionKey::eMouseLeft);
    InputMapper::getInstance()->CreateAction("CameraRotation", EActionKey::eCursorDelta);
    InputMapper::getInstance()->CreateAction("CameraMovementToPoint", EActionKey::eScrol);

    InputMapper::getInstance()->BindAction("CameraMovement", EKeyState::ePressed, this, &CameraEditorController::CameraMovement);
    InputMapper::getInstance()->BindAxis("CameraRotation", this, &CameraEditorController::MouseRotation);
    InputMapper::getInstance()->BindAxis("CameraMovementToPoint", this, &CameraEditorController::CameraToPoint);
}

void CameraEditorController::Update(float fDeltaTime)
{
    CameraController::Update(fDeltaTime);
}

void CameraEditorController::CameraMovement(EActionKey eKey, EKeyState eState)
{
    auto camera = Components::CameraManager::getInstance()->GetCurrentCamera();
    FTransform transform = camera->GetTransform();

    glm::vec3 direction{0.f};
    switch (eKey)
    {
    case EActionKey::eW:
        direction += transform.GetForwardVector();
        break;
    case EActionKey::eS:
        direction -= transform.GetForwardVector();
        break;
    case EActionKey::eA:
        direction -= transform.GetRightVector();
        break;
    case EActionKey::eD:
        direction += transform.GetRightVector();
        break;
    case EActionKey::eSpace:
        direction += transform.GetUpVector();
        break;
    case EActionKey::eLeftControl:
        direction -= transform.GetUpVector();
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
        camera->SetTransform(transform);
    }
}

void CameraEditorController::MouseRotation(float fX, float fY)
{
    if (!m_bRotatePass)
        return;

    auto camera = Components::CameraManager::getInstance()->GetCurrentCamera();
    FTransform transform = camera->GetTransform();
    glm::vec3 rotate = glm::vec3{fY * m_fLookSpeed * -1.f, fX * m_fLookSpeed, 0.f};

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
    {
        transform.rot += rotate * 2.0f;

        /*transform.rot.x = glm::clamp(transform.rot.x, -1.5f, 1.5f);
        transform.rot.y = glm::mod(transform.rot.y, glm::two_pi<float>());*/

        camera->SetTransform(transform);
    }
    m_bRotatePass = false;
}

void CameraEditorController::CameraToPoint(float fX, float fY)
{
    auto camera = Components::CameraManager::getInstance()->GetCurrentCamera();
    FTransform transform = camera->GetTransform();
    glm::vec3 direction = transform.GetForwardVector() * fY;

    if (glm::dot(direction, direction) > std::numeric_limits<float>::epsilon())
    {
        transform.pos += m_fScrollSpeed * direction;
        camera->SetTransform(transform);
    }
}