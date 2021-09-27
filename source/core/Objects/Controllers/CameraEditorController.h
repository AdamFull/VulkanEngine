#pragma once
#include "CameraController.h"

namespace Engine
{
    class CameraEditorController : public CameraController
    {
    public:
        void Create(std::unique_ptr<Device>& device) override;
        void Update(float fDeltaTime, std::unique_ptr<SwapChain>& swapchain) override;
        void Render(float fDeltaTime, vk::CommandBuffer& commandBuffer) override;
    private:
        void CameraMovement(EActionKey eKey);
        void MouseRotation(float fX, float fY);
        void CameraToPoint(float fX, float fY);

        bool m_bRotatePass{false};
    };
}