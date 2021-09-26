#include "CameraController.h"
#include "InputMapper.h"
#include "Camera.h"

namespace Engine
{
    void CameraController::Initialize(std::unique_ptr<InputMapper>& mapper)
    {
        mapper->CreateAction("CameraMovement", EActionKey::eW, EActionKey::eS, EActionKey::eA, 
        EActionKey::eD, EActionKey::eSpace, EActionKey::eLeftControl, EActionKey::eMouseMiddle);
        mapper->CreateAction("CameraRotation", EActionKey::eCursorDelta);
        mapper->CreateAction("CameraMovementToPoint", EActionKey::eScrol);
        
        mapper->BindAction("CameraMovement", EKeyState::ePressed, this, &CameraController::CameraMovement);
        mapper->BindAxis("CameraRotation", this, &CameraController::MouseRotation);
        mapper->BindAxis("CameraMovementToPoint", this, &CameraController::CameraToPoint);

        m_pCamera = std::make_shared<CameraBase>();
    }

    void CameraController::Update(float fDeltaTime)
    {
        m_fDeltaTime = fDeltaTime;
    }

    void CameraController::CameraMovement(EActionKey eKey)
    {
        FTransform transform = m_pCamera->GetTransform();

        glm::vec3 direction{0.f};
        switch (eKey)
        {
            case EActionKey::eW: direction += m_pCamera->GetForwardVector(); break;
            case EActionKey::eS: direction -= m_pCamera->GetForwardVector(); break;
            case EActionKey::eA: direction -= m_pCamera->GetRightVector(); break;
            case EActionKey::eD: direction += m_pCamera->GetRightVector(); break;
            case EActionKey::eSpace: direction += m_pCamera->GetUpVector(); break;
            case EActionKey::eLeftControl: direction -= m_pCamera->GetUpVector(); break;
            case EActionKey::eMouseMiddle: m_bRotatePass = true; break;
            
            default:
                break;
        }

        if (glm::dot(direction, direction) > std::numeric_limits<float>::epsilon())
        {
            transform.pos += m_fMoveSpeed * m_fDeltaTime * glm::normalize(direction);
        }
        
        m_pCamera->SetViewYXZ(transform);
    }

    void CameraController::MouseRotation(float fX, float fY)
    {
        if(!m_bRotatePass)
            return;

        FTransform transform = m_pCamera->GetTransform();
        glm::vec3 rotate = glm::vec3{fY * m_fLookSpeed * -1.f, fX * m_fLookSpeed, 0.f};

        if(glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        {
            transform.rot += rotate;
        }

        transform.rot.x = glm::clamp(transform.rot.x, -1.5f, 1.5f);
        transform.rot.y = glm::mod(transform.rot.y, glm::two_pi<float>());

        m_pCamera->SetViewYXZ(transform);
        m_bRotatePass = false;
    }

    void CameraController::CameraToPoint(float fX, float fY)
    {
        FTransform transform = m_pCamera->GetTransform();
        glm::vec3 direction = m_pCamera->GetForwardVector() * fY;

        if (glm::dot(direction, direction) > std::numeric_limits<float>::epsilon())
        {
            transform.pos += m_fScrollSpeed * direction;
        }
        
        m_pCamera->SetViewYXZ(transform);
    }
}