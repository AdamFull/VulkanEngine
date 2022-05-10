#pragma once
#include "keymapper/KeycodeConfig.h"

namespace Engine
{
    namespace Core
    {
        
        
    }

    namespace Controllers
    {
        class CCameraController
        {
        public:
            virtual void create();
            virtual void update(float fDeltaTime);

            // Getters
        protected:
            float m_fDeltaTime{0.f};
            float m_fMoveSpeed{10.f};
            float m_fScrollSpeed{100.f};
            float m_fLookSpeed{1000.f};
        };
    }
}