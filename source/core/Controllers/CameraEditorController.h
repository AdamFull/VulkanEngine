#pragma once
#include "CameraController.h"

namespace Engine
{
    namespace Controllers
    {
        class CCameraEditorController : public CCameraController
        {
        public:
            void create() override;
            void update(float fDeltaTime) override;

        private:
            void cameraMovement(EActionKey eKey, EKeyState eState);
            void mouseRotation(float fX, float fY);
            void cameraToPoint(float fX, float fY);

            bool m_bRotatePass{false};
        };
    }
}