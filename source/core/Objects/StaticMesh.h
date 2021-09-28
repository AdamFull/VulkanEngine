#pragma once
#include "RenderObject.h"

namespace Engine
{
    class VulkanStaticMesh;

    class StaticMesh : public RenderObject
    {
    public:
        explicit StaticMesh(std::string srName) 
        {
            m_srName = srName;
        }
        
        void Create(std::unique_ptr<Device>& device) override;
        void Update(float fDeltaTime, std::unique_ptr<SwapChain>& swapchain) override;
        void Render(float fDeltaTime, vk::CommandBuffer& commandBuffer) override;

        inline std::unique_ptr<VulkanStaticMesh>& GetMesh() { return m_pStaticMesh; }
    private:
        std::unique_ptr<VulkanStaticMesh> m_pStaticMesh;
    };
}