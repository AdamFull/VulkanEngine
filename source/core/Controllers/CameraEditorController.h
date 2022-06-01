#pragma once
#include "CameraController.h"

namespace engine
{
    namespace controllers
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