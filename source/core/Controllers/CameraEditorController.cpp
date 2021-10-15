#include "CameraEditorController.h"
#include "KeyMapping/InputMapper.h"
#include "Objects/Components/Camera/CameraComponent.h"
#include "Objects/Components/Camera/CameraManager.h"

namespace Engine
{
    void CameraEditorController::Create()
    {

        InputMapper::GetInstance()->CreateAction("CameraMovement", EActionKey::eW, EActionKey::eS, EActionKey::eA,
                                                 EActionKey::eD, EActionKey::eSpace, EActionKey::eLeftControl, EActionKey::eMouseMiddle);
        InputMapper::GetInstance()->CreateAction("CameraRotation", EActionKey::eCursorDelta);
        InputMapper::GetInstance()->CreateAction("CameraMovementToPoint", EActionKey::eScrol);

        InputMapper::GetInstance()->BindAction("CameraMovement", EKeyState::ePressed, this, &CameraEditorController::CameraMovement);
        InputMapper::GetInstance()->BindAxis("CameraRotation", this, &CameraEditorController::MouseRotation);
        InputMapper::GetInstance()->BindAxis("CameraMovementToPoint", this, &CameraEditorController::CameraToPoint);
    }

    void CameraEditorController::Update(float fDeltaTime)
    {
        CameraController::Update(fDeltaTime);
    }

    void CameraEditorController::CameraMovement(EActionKey eKey, EKeyState eState)
    {
        auto camera = CameraManager::GetInstance()->GetCurrentCamera();
        FTransform transform = camera->GetTransform();

        glm::vec3 direction{0.f};
        switch (eKey)
        {
        case EActionKey::eW:
            direction += camera->GetForwardVector();
            break;
        case EActionKey::eS:
            direction -= camera->GetForwardVector();
            break;
        case EActionKey::eA:
            direction -= camera->GetRightVector();
            break;
        case EActionKey::eD:
            direction += camera->GetRightVector();
            break;
        case EActionKey::eSpace:
            direction += camera->GetUpVector();
            break;
        case EActionKey::eLeftControl:
            direction -= camera->GetUpVector();
            break;
        case EActionKey::eMouseMiddle:
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

        auto camera = CameraManager::GetInstance()->GetCurrentCamera();
        FTransform transform = camera->GetTransform();
        glm::vec3 rotate = glm::vec3{fY * m_fLookSpeed * -1.f, fX * m_fLookSpeed, 0.f};

        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        {
            transform.rot += rotate;

            transform.rot.x = glm::clamp(transform.rot.x, -1.5f, 1.5f);
            transform.rot.y = glm::mod(transform.rot.y, glm::two_pi<float>());

            camera->SetTransform(transform);
        }
        m_bRotatePass = false;
    }

    void CameraEditorController::CameraToPoint(float fX, float fY)
    {
        auto camera = CameraManager::GetInstance()->GetCurrentCamera();
        FTransform transform = camera->GetTransform();
        glm::vec3 direction = camera->GetForwardVector() * fY;

        if (glm::dot(direction, direction) > std::numeric_limits<float>::epsilon())
        {
            transform.pos += m_fScrollSpeed * direction;
            camera->SetTransform(transform);
        }
    }
}