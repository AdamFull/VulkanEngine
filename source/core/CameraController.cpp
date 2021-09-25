#include "CameraController.h"
#include "InputMapper.h"
#include "Camera.h"

namespace Engine
{
    void CameraController::Initialize(std::unique_ptr<InputMapper>& mapper)
    {
        mapper->BindAction(EActionKey::eW, EKeyState::ePressed, this, &CameraController::MoveForward);
        mapper->BindAction(EActionKey::eS, EKeyState::ePressed, this, &CameraController::MoveBackward);
        mapper->BindAction(EActionKey::eA, EKeyState::ePressed, this, &CameraController::MoveLeft);
        mapper->BindAction(EActionKey::eD, EKeyState::ePressed, this, &CameraController::MoveRight);
        mapper->BindAction(EActionKey::eQ, EKeyState::ePressed, this, &CameraController::MoveUp);
        mapper->BindAction(EActionKey::eE, EKeyState::ePressed, this, &CameraController::MoveDown);

        mapper->BindAxis(EActionKey::eCursorDelta, this, &CameraController::MouseRotation);
    }

    void CameraController::AttachCamera(std::unique_ptr<CameraBase>& camera)
    {
        ViewRotation = EasyDelegate::TDelegate<void(glm::vec3, glm::vec3)>(camera.get(), &CameraBase::SetViewYXZ);
    }

    void CameraController::MoveForward()
    {
        m_vecPosition.z += 0.0005;
        if(ViewRotation)
        {
            ViewRotation(m_vecPosition, m_vecRotation);
        }
    }

    void CameraController::MoveBackward()
    {
        m_vecPosition.z += -0.0005;
        if(ViewRotation)
        {
            ViewRotation(m_vecPosition, m_vecRotation);
        }
    }

    void CameraController::MoveLeft()
    {
        m_vecPosition.x += 0.0005;
        if(ViewRotation)
        {
            ViewRotation(m_vecPosition, m_vecRotation);
        }
    }

    void CameraController::MoveRight()
    {
        m_vecPosition.x += -0.0005;
        if(ViewRotation)
        {
            ViewRotation(m_vecPosition, m_vecRotation);
        }
    }

    void CameraController::MoveUp()
    {
        m_vecPosition.y += 0.0005;
        if(ViewRotation)
        {
            ViewRotation(m_vecPosition, m_vecRotation);
        }
    }

    void CameraController::MoveDown()
    {
        m_vecPosition.y += -0.0005;
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