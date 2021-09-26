#include "CameraController.h"
#include "InputMapper.h"
#include "Camera.h"

namespace Engine
{
    void CameraController::Initialize(std::unique_ptr<InputMapper>& mapper)
    {
        mapper->CreateAction("CameraMovement", EActionKey::eW, EActionKey::eS, EActionKey::eA, EActionKey::eD, EActionKey::eSpace, EActionKey::eLeftControl);
        mapper->CreateAction("CameraRotation", EActionKey::eCursorDelta);
        
        mapper->BindAction("CameraMovement", EKeyState::ePressed, this, &CameraController::CameraMovement);
        mapper->BindAxis("CameraRotation", this, &CameraController::MouseRotation);
    }

    void CameraController::AttachCamera(std::unique_ptr<CameraBase>& camera)
    {
        ViewRotation = EasyDelegate::TDelegate<void(glm::vec3, glm::vec3)>(camera.get(), &CameraBase::SetViewYXZ);
    }

    void CameraController::Update(float fDeltaTime)
    {
        m_fDeltaTime = fDeltaTime;
    }

    void CameraController::CameraMovement(EActionKey eKey)
    {
        //TODO: Move forward vector and right vector to camera
        float yaw = transform.rot.y;
        const glm::vec3 forwardVector{sin(yaw), 0.f, cos(yaw)};
        const glm::vec3 rightVector{forwardVector.z, 0.f, -forwardVector.x};
        const glm::vec3 upVector{0.f, -1.f, 0.f};

        glm::vec3 direction{0.f};
        switch (eKey)
        {
        case EActionKey::eW: direction += forwardVector; break;
        case EActionKey::eS: direction -= forwardVector; break;
        case EActionKey::eA: direction += rightVector; break;
        case EActionKey::eD: direction -= rightVector; break;
        case EActionKey::eSpace: direction += upVector; break;
        case EActionKey::eLeftControl: direction -= upVector; break;
        
        default:
            break;
        }

        if (glm::dot(direction, direction) > std::numeric_limits<float>::epsilon())
        {
            transform.pos += m_fMoveSpeed * m_fDeltaTime * glm::normalize(direction);
        }
        
        if(ViewRotation)
        {
            ViewRotation(transform.pos, transform.rot);
        }
    }

    void CameraController::MouseRotation(float fX, float fY)
    {
        glm::vec3 rotate = glm::vec3{fY * m_fLookSpeed * -1.f, fX * m_fLookSpeed, 0.f};

        if(glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        {
            transform.rot += rotate;
        }

        transform.rot.x = glm::clamp(transform.rot.x, -1.5f, 1.5f);
        transform.rot.y = glm::mod(transform.rot.y, glm::two_pi<float>());

        if (ViewRotation)
        {
            ViewRotation(transform.pos, transform.rot);
        }
    }
}