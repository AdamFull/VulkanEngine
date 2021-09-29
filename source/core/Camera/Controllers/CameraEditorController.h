#pragma once
#include "CameraController.h"

namespace Engine
{
    class Device;
    class SwapChain;
    class UniformBuffer;
    
    class CameraEditorController : public CameraController
    {
    public:
        explicit CameraEditorController(std::string srName) 
        {
            
        }
        
        void Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform) override;
        void Update(float fDeltaTime) override;
        void Render(float fDeltaTime, vk::CommandBuffer& commandBuffer) override;
    private:
        void CameraMovement(EActionKey eKey);
        void MouseRotation(float fX, float fY);
        void CameraToPoint(float fX, float fY);

        bool m_bRotatePass{false};
    };
}