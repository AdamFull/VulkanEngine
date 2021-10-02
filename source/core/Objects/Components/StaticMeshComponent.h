#pragma once
#include "Objects/RenderObject.h"

namespace Engine
{
    class StaticMesh;
    class MaterialBase;

    class StaticMeshComponent : public RenderObject
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

        void SetMesh(std::shared_ptr<StaticMesh> mesh) { m_pStaticMesh = mesh; }
        void SetMaterial(std::shared_ptr<MaterialBase> material) { m_pMaterial = material; }

        inline std::shared_ptr<StaticMesh> GetMesh() { return m_pStaticMesh; }
    private:
        std::shared_ptr<StaticMesh> m_pStaticMesh;
        std::shared_ptr<MaterialBase> m_pMaterial;
    };
}