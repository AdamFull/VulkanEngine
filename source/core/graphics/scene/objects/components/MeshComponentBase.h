#pragma once
#include "graphics/scene/objects/RenderObject.h"

namespace Engine
{
    namespace Resources
    {
        namespace Mesh
        {
            class CMeshBase;
        }
    }
    namespace Core
    {
        namespace Scene
        {
            class CMeshObjectBase : public CRenderObject
            {
            public:
                CMeshObjectBase();
                void create() override;
                void reCreate() override;
                void update(float fDeltaTime) override;
                void render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;
                void cleanup() override;
                void destroy() override;

                virtual void setMesh(ref_ptr<Resources::Mesh::CMeshBase> mesh) { m_pMesh = mesh; }
                virtual void setIndex(uint32_t index) { m_iIndex = index; }
                void setInstances(const std::vector<glm::vec4> &vInstances);

            protected:
                std::vector<glm::vec4> m_vInstances;
                ref_ptr<Resources::Mesh::CMeshBase> m_pMesh;
                uint32_t m_iIndex{0};
            };
        }
    }
}