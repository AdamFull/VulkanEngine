#pragma once
#include "CameraController.h"

namespace Engine
{
    class CameraEditorController : public CameraController
    {
    public:
        void Create() override;
        void Update(float fDeltaTime) override;
    private:
        void CameraMovement(EActionKey eKey, EKeyState eState);
        void MouseRotation(float fX, float fY);
        void CameraToPoint(float fX, float fY);

        bool m_bRotatePass{false};
    };
}