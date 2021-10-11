#pragma once
#include "ComponentBase.h"

namespace Engine
{
    class MeshBase;
    class StaticMeshComponent : public ComponentBase
    {
    public:
        StaticMeshComponent() = default;
        explicit StaticMeshComponent(std::string srName) 
        {
            m_srName = srName;
        }
        
        void Create() override;
        void ReCreate() override;
        void Update(float fDeltaTime) override;
        void Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;
    private:
        std::shared_ptr<MeshBase> m_pStaticMesh;
    };
}