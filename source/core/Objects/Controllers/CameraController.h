#pragma once
#include "core/KeycodeConfig.h"
#include "core/Objects/RenderObject.h"

namespace Engine
{
    class CameraController : public RenderObject
    {
    public:
        CameraController() = default;
        explicit CameraController(std::string srName) 
        {
            m_srName = srName;
        }
        void Create(std::unique_ptr<Device>& device) override;
        void Update(float fDeltaTime, std::unique_ptr<SwapChain>& swapchain) override;
        void Render(float fDeltaTime, vk::CommandBuffer& commandBuffer) override;

        //TODO: make it better
        virtual inline std::shared_ptr<RenderObject> GetCamera(std::string srName) { return GetChilds()[srName]; }

        //Getters
    protected:
        float m_fDeltaTime{ 0.f };
        float m_fMoveSpeed{ 5.f };
        float m_fScrollSpeed{ 100.f };
        float m_fLookSpeed{ 1000.f };
    };
}