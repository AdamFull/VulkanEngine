#pragma once
#include "core/Objects/RenderObject.h"

namespace Engine
{
    class Device;

    class SceneRootComponent : public RenderObject
    {
    public:
        explicit SceneRootComponent(std::string srName = "SceneRoot") 
        {
            m_srName = srName;
        }
        
        void Create(std::unique_ptr<Device>& device) override;
        void Render(float fDeltaTime, vk::CommandBuffer& commandBuffer) override;
        void Update(float fDeltaTime, std::unique_ptr<SwapChain>& swapchain) override;
    };
}