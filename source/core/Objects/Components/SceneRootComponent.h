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
        
        void Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform) override;
        void Render(float fDeltaTime, vk::CommandBuffer& commandBuffer) override;
        void Update(float fDeltaTime) override;
    };
}