#include "CameraController.h"

namespace Engine
{
    void CameraController::Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform)
    {
        RenderObject::Create(device, swapchain, uniform);
    }

    void CameraController::Update(float fDeltaTime)
    {
        RenderObject::Update(fDeltaTime);
        m_fDeltaTime = fDeltaTime;
    }

    void CameraController::Render(float fDeltaTime, vk::CommandBuffer& commandBuffer)
    {
        RenderObject::Render(fDeltaTime, commandBuffer);
    }
}