#pragma once
#include "Objects/RenderObject.h"

namespace Engine
{
    class MeshBase;
    class ResourceManager;

    class StaticMeshComponent : public RenderObject
    {
    public:
        StaticMeshComponent() = default;
        explicit StaticMeshComponent(std::string srName) 
        {
            m_srName = srName;
        }
        
        void Create(std::shared_ptr<ResourceManager> resourceMgr) override;
        void ReCreate() override;
        void Update(float fDeltaTime) override;
        void Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;
    private:
        std::shared_ptr<MeshBase> m_pStaticMesh;
        std::shared_ptr<UniformBuffer<FUniformData>> m_pUniform;
    };
}