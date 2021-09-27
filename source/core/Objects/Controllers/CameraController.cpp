#include "CameraController.h"

namespace Engine
{
    void CameraController::Create(std::unique_ptr<Device>& device)
    {
        RenderObject::Create(device);
    }

    void CameraController::Update(float fDeltaTime, std::unique_ptr<SwapChain>& swapchain)
    {
        RenderObject::Update(fDeltaTime, swapchain);
        m_fDeltaTime = fDeltaTime;
    }

    void CameraController::Render(float fDeltaTime, vk::CommandBuffer& commandBuffer)
    {
        RenderObject::Render(fDeltaTime, commandBuffer);
    }
}