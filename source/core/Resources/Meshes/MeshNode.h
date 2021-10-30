#pragma once
#include "Resources/ResourceBase.h"

namespace Engine
{
    namespace Resources
    {
        namespace Mesh
        {
            class MeshBase;
            class MeshNode : public ResourceBase
            {
            public:
                void Create() override;
                void ReCreate() override;
                void Update(uint32_t imageIndex) override;
                void Cleanup() override;
                void Destroy() override;

                void SetTranslation(glm::vec3&& translation);
                void SetScale(glm::vec3&& scale);
                void SetRotation(glm::quat&& rotation);
                void SetLocalMatrix(glm::mat4&& matrix);
                glm::mat4 GetLocalMatrix();
            protected:
                glm::vec3 m_vTranslation{};
		        glm::vec3 m_vScale{1.0f};
		        glm::quat m_qRotation{};
                glm::mat4 m_mMatrix;
		
                std::shared_ptr<MeshBase> m_pMesh;
            };
        }
    }
}