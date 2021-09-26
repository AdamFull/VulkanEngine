#include "CameraController.h"
#include "InputMapper.h"
#include "Camera.h"

namespace Engine
{
    void CameraController::Initialize(std::unique_ptr<InputMapper>& mapper)
    {
        mapper->CreateAction("CameraMovement", EActionKey::eW, EActionKey::eS, EActionKey::eA, EActionKey::eD, EActionKey::eQ, EActionKey::eE);
        mapper->CreateAction("CameraRotation", EActionKey::eCursorDelta);
        
        mapper->BindAction("CameraMovement", EKeyState::ePressed, this, &CameraController::CameraMovement);
        mapper->BindAxis("CameraRotation", this, &CameraController::MouseRotation);
    }

    void CameraController::AttachCamera(std::unique_ptr<CameraBase>& camera)
    {
        ViewRotation = EasyDelegate::TDelegate<void(glm::vec3, glm::vec3)>(camera.get(), &CameraBase::SetViewYXZ);
    }

    void CameraController::CameraMovement(EActionKey eKey)
    {
        switch (eKey)
        {
        case EActionKey::eW: m_vecPosition.z += 0.0005; break;
        case EActionKey::eS: m_vecPosition.z += -0.0005; break;
        case EActionKey::eA: m_vecPosition.x += 0.0005; break;
        case EActionKey::eD: m_vecPosition.x += -0.0005; break;
        case EActionKey::eQ: m_vecPosition.y += 0.0005; break;
        case EActionKey::eE: m_vecPosition.y += -0.0005; break;
        
        default:
            break;
        }
        
        if(ViewRotation)
        {
            ViewRotation(m_vecPosition, m_vecRotation);
        }
    }

    void CameraController::MouseRotation(float fX, float fY)
    {
        m_vecRotation += glm::vec3{fY * m_fLookSpeed * -1.f, fX * m_fLookSpeed, 0.f};
        if(glm::dot(m_vecRotation, m_vecRotation) > std::numeric_limits<float>::epsilon())
        {
            if(ViewRotation)
            {
                ViewRotation(m_vecPosition, m_vecRotation);
            }
        }
        
    }
}