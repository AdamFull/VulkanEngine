#include "CameraController.h"
#include "InputMapper.h"
#include "Camera.h"

namespace Engine
{
    void CameraController::Initialize(std::unique_ptr<InputMapper>& mapper)
    {
        mapper->BindAction(EActionKey::eW, EKeyState::ePressed, EasyDelegate::TDelegate<void()>(this, &CameraController::MoveForward));
        mapper->BindAction(EActionKey::eS, EKeyState::ePressed, EasyDelegate::TDelegate<void()>(this, &CameraController::MoveBackward));
        mapper->BindAction(EActionKey::eA, EKeyState::ePressed, EasyDelegate::TDelegate<void()>(this, &CameraController::MoveLeft));
        mapper->BindAction(EActionKey::eD, EKeyState::ePressed, EasyDelegate::TDelegate<void()>(this, &CameraController::MoveRight));
        mapper->BindAction(EActionKey::eQ, EKeyState::ePressed, EasyDelegate::TDelegate<void()>(this, &CameraController::MoveUp));
        mapper->BindAction(EActionKey::eE, EKeyState::ePressed, EasyDelegate::TDelegate<void()>(this, &CameraController::MoveDown));

        mapper->BindAxis(EActionKey::eCursorSpeedX, EasyDelegate::TDelegate<void(float)>(this, &CameraController::MouseRotationX));
        mapper->BindAxis(EActionKey::eCursorSpeedY, EasyDelegate::TDelegate<void(float)>(this, &CameraController::MouseRotationY));
    }

    void CameraController::BindView(EasyDelegate::TDelegate<void(glm::vec3, glm::vec3)>&& viewRot)
    {
        ViewRotation = std::move(viewRot);
    }

    void CameraController::MoveForward()
    {
        m_vecPosition.x += -0.001;
        if(ViewRotation)
        {
            ViewRotation(m_vecPosition, m_vecRotation);
        }
    }

    void CameraController::MoveBackward()
    {
        m_vecPosition.x += 0.001;
        if(ViewRotation)
        {
            ViewRotation(m_vecPosition, m_vecRotation);
        }
    }

    void CameraController::MoveLeft()
    {
        m_vecPosition.y += -0.001;
        if(ViewRotation)
        {
            ViewRotation(m_vecPosition, m_vecRotation);
        }
    }

    void CameraController::MoveRight()
    {
        m_vecPosition.y += 0.001;
        if(ViewRotation)
        {
            ViewRotation(m_vecPosition, m_vecRotation);
        }
    }

    void CameraController::MoveUp()
    {
        m_vecPosition.z += -0.001;
        if(ViewRotation)
        {
            ViewRotation(m_vecPosition, m_vecRotation);
        }
    }

    void CameraController::MoveDown()
    {
        m_vecPosition.z += 0.001;
        if(ViewRotation)
        {
            ViewRotation(m_vecPosition, m_vecRotation);
        }
    }

    void CameraController::MouseRotationX(float fX)
    {
        /*m_vecRotation = {};

        m_vecRotation.x += fX + m_fLookSpeed;
        if(glm::dot(m_vecRotation, m_vecRotation) > std::numeric_limits<float>::epsilon())
        {
            if(ViewRotation)
            {
                ViewRotation(m_vecPosition, m_vecRotation);
            }
        }*/
        
    }

    void CameraController::MouseRotationY(float fY)
    {
        /*m_vecRotation = {};

        m_vecRotation.x += fY + m_fLookSpeed;
        if(glm::dot(m_vecRotation, m_vecRotation) > std::numeric_limits<float>::epsilon())
        {
            if(ViewRotation)
            {
                ViewRotation(m_vecPosition, m_vecRotation);
            }
        }*/
    }
}