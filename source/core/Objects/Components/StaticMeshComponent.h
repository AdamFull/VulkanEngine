#pragma once
#include "Objects/RenderObject.h"

namespace Engine
{
    class StaticMesh;
    class MaterialBase;

    class StaticMeshComponent : public RenderObject
    {
    public:
        explicit StaticMeshComponent(std::string srName) 
        {
            m_srName = srName;
        }
        
        void Create() override;
        void Update(float fDeltaTime) override;
        void Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex) override;

        inline std::unique_ptr<StaticMesh>& GetMesh() { return m_pStaticMesh; }
    private:
        std::unique_ptr<StaticMesh> m_pStaticMesh;
        std::shared_ptr<MaterialBase> m_pMaterial;
    };
}