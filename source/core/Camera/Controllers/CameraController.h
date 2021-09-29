#pragma once
#include "core/KeyMapping/KeycodeConfig.h"

namespace Engine
{
    class Device;
    class SwapChain;
    class UniformBuffer;

    class CameraController
    {
    public:
        virtual void Create();
        virtual void Update(float fDeltaTime);

        //Getters
    protected:
        float m_fDeltaTime{ 0.f };
        float m_fMoveSpeed{ 5.f };
        float m_fScrollSpeed{ 100.f };
        float m_fLookSpeed{ 1000.f };
    };
}