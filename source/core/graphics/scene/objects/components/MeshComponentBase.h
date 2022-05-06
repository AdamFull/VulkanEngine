#pragma once
#include "ComponentBase.h"

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
            class CMeshComponentBase : public CComponentBase
            {
            public:
                void create(vk::RenderPass &renderPass, uint32_t subpass) override;
                void reCreate() override;
                void update(float fDeltaTime) override;
                void render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;
                void cleanup() override;
                void destroy() override;

                virtual void setMesh(std::shared_ptr<Resources::Mesh::CMeshBase> mesh) { m_pMesh = mesh; }
                virtual void setIndex(uint32_t index) { m_iIndex = index; }
                void setInstances(const std::vector<glm::vec4> &vInstances);

            protected:
                std::vector<glm::vec4> m_vInstances;
                std::shared_ptr<Resources::Mesh::CMeshBase> m_pMesh;
                uint32_t m_iIndex{0};
            };
        }
    }
}