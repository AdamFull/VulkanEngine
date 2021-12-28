#pragma once
#include "ComponentBase.h"

namespace Engine
{
    namespace Resources
    {
        namespace Mesh
        {
            class MeshBase;
        }
    }
    namespace Core
    {
        namespace Scene
        {
            namespace Objects
            {
                namespace Components
                {
                    class MeshComponentBase : public ComponentBase
                    {
                    public:
                        void Create(std::shared_ptr<Resources::ResourceManager> resourceMgr) override;
                        void ReCreate() override;
                        void Update(float fDeltaTime) override;
                        void Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;
                        void Cleanup() override;
                        void Destroy() override;

                        virtual void SetMesh(std::shared_ptr<Resources::Mesh::MeshBase> mesh) { m_pMesh = mesh; }
                        virtual void SetIndex(uint32_t index) { m_iIndex = index; }
                        void SetInstances(const std::vector<glm::vec4>& vInstances) { m_vInstances = vInstances; }

                    protected:
                        std::vector<glm::vec4> m_vInstances;
                        std::shared_ptr<Resources::Mesh::MeshBase> m_pMesh;
                        uint32_t m_iIndex{0};
                    };
                }
            }
        }
    }
}