#pragma once
#include "KeycodeConfig.h"

namespace Engine
{
    class InputMapper;
    class CameraBase;

    class CameraController
    {
    public:
        void Initialize(std::unique_ptr<InputMapper>& mapper);
        void Update(float fDeltaTime);

        inline std::shared_ptr<CameraBase>& GetCamera() { return m_pCamera; }
        void UpdateProjection(float fAspectRatio);

    private:
        void CameraMovement(EActionKey eKey);
        void MouseRotation(float fX, float fY);
        void CameraToPoint(float fX, float fY);

        std::shared_ptr<CameraBase> m_pCamera;

        bool m_bRotatePass{false};
        float m_fDeltaTime{ 0.f };
        float m_fMoveSpeed{ 5.f };
        float m_fScrollSpeed{ 100.f };
        float m_fLookSpeed{ 1000.f };
    };
}