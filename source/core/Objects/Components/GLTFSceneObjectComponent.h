#pragma once
#include "Objects/RenderObject.h"

namespace Engine
{
    // Create mesh component base
    class MeshBase;
    class GLTFSceneObjectComponent : public RenderObject
    {
    public:
        void Create(std::shared_ptr<ResourceManager> resourceMgr) override;
        void ReCreate() override;
        void Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex) override;
        void Update(float fDeltaTime) override;
        void Cleanup() override;
        void Destroy() override;

        void SetMesh(std::shared_ptr<MeshBase> mesh) { m_pMesh = mesh; }
        void SetIndex(uint32_t index);
    protected:
        std::shared_ptr<MeshBase> m_pMesh;
        uint32_t m_iIndex{0};
    };
}