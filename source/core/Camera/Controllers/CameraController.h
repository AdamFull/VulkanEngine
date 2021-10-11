#pragma once
#include "KeyMapping/KeycodeConfig.h"

namespace Engine
{
    class Device;
    class SwapChain;

    class CameraController
    {
    public:
        virtual void Create();
        virtual void Update(float fDeltaTime);

        //Getters
    protected:
        float m_fDeltaTime{ 0.f };
        float m_fMoveSpeed{ 2.f };
        float m_fScrollSpeed{ 100.f };
        float m_fLookSpeed{ 1000.f };
    };
}