#pragma once
#include "KeyMapping/KeycodeConfig.h"

namespace Engine
{
    namespace Core
    {
        class Device;
        class SwapChain;
    }

    namespace Controllers
    {
        class CameraController
        {
        public:
            virtual void Create();
            virtual void Update(float fDeltaTime);

            // Getters
        protected:
            float m_fDeltaTime{0.f};
            float m_fMoveSpeed{2.f};
            float m_fScrollSpeed{100.f};
            float m_fLookSpeed{1000.f};
        };
    }
}