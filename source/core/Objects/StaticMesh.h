#pragma once
#include "RenderObject.h"

namespace Engine
{
    class VulkanStaticMesh;
    class MaterialBase;

    class StaticMesh : public RenderObject
    {
    public:
        explicit StaticMesh(std::string srName) 
        {
            m_srName = srName;
        }
        
        void Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform) override;
        void Update(float fDeltaTime) override;
        void Render(float fDeltaTime, vk::CommandBuffer& commandBuffer) override;

        inline std::unique_ptr<VulkanStaticMesh>& GetMesh() { return m_pStaticMesh; }
    private:
        std::unique_ptr<VulkanStaticMesh> m_pStaticMesh;
        std::shared_ptr<MaterialBase> m_pMaterial;
    };
}