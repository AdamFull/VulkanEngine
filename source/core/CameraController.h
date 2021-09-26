#pragma once
#include "KeycodeConfig.h"
#include "DataTypes/VulkanVertex.h"

namespace Engine
{
    class InputMapper;
    class CameraBase;

    class CameraController
    {
    public:
        void Initialize(std::unique_ptr<InputMapper>& mapper);
        void AttachCamera(std::unique_ptr<CameraBase>& camera);

        void Update(float fDeltaTime);

    private:
        void CameraMovement(EActionKey eKey);
        void MouseRotation(float fX, float fY);

        FTransform transform;

        EasyDelegate::TDelegate<void(glm::vec3, glm::vec3)> ViewRotation;

        float m_fDeltaTime{0.f};
        float m_fMoveSpeed{3.f};
        float m_fLookSpeed{500.f};
    };
}