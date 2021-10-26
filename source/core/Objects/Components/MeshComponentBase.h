#pragma once
#include "ComponentBase.h"

namespace Engine
{
    class MeshBase;
    class MeshComponentBase : public ComponentBase
    {
    public:
        void Create(std::shared_ptr<ResourceManager> resourceMgr) override;
        void ReCreate() override;
        void Update(float fDeltaTime) override;
        void Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;

        virtual void SetMesh(std::shared_ptr<MeshBase> mesh) { m_pMesh = mesh; }
        virtual void SetIndex(uint32_t index) { m_iIndex = index; }
    protected:
        std::shared_ptr<MeshBase> m_pMesh;
        uint32_t m_iIndex{0};
    };
}