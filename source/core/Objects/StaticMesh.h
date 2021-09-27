#pragma once
#include "RenderObject.h"

namespace Engine
{
    class VulkanStaticMesh;

    class StaticMesh : public RenderObject
    {
    public:
        ~StaticMesh() {}
        void Create(std::unique_ptr<Device>& device) override;
        void Update(float fDeltaTime) override;
        void Render(float fDeltaTime, vk::CommandBuffer& commandBuffer) override;

        inline std::unique_ptr<VulkanStaticMesh>& GetMesh() { return m_pStaticMesh; }
    private:
        std::unique_ptr<VulkanStaticMesh> m_pStaticMesh;
    };
}