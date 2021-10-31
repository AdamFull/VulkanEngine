#pragma once

namespace Engine
{
    namespace Resources
    {
        namespace Mesh
        {
            class MeshBase;
            class MeshNode;
            struct Skin
            {
                std::string name;
                std::shared_ptr<MeshNode> skeletonRoot = nullptr;
                std::vector<glm::mat4> inverseBindMatrices;
                std::vector<std::shared_ptr<MeshNode>> joints;
            };

            class MeshNode : public std::enable_shared_from_this<MeshNode>
            {
            public:
                void Create();
                void ReCreate();
                void Update(uint32_t imageIndex);
                void Cleanup();
                void Destroy();

                void SetTranslation(glm::vec3&& translation);
                void SetScale(glm::vec3&& scale);
                void SetRotation(glm::quat&& rotation);
                void SetLocalMatrix(glm::mat4&& matrix);
                glm::mat4 GetLocalMatrix();
                glm::mat4 GetMatrix();

                inline void SetName(std::string srName) { m_srName = srName; }
                inline std::string &GetName() { return m_srName; }

                void SetParent(std::shared_ptr<MeshNode> parent);
                void Attach(std::shared_ptr<MeshNode> child);
                void Detach(std::shared_ptr<MeshNode> child);

                inline std::shared_ptr<MeshNode> GetParent() { return m_pParent; }
                inline std::map<std::string, std::shared_ptr<MeshNode>> &GetChilds() { return m_mChilds; }
                void AddChild(std::shared_ptr<MeshNode> child);
            protected:
                std::string m_srName;
                glm::vec3 m_vTranslation{};
		        glm::vec3 m_vScale{1.0f};
		        glm::quat m_qRotation{};
                glm::mat4 m_mMatrix;
		
                std::shared_ptr<MeshBase> m_pMesh;
                std::unique_ptr<Skin> m_pSkin;
            private:
                std::shared_ptr<MeshNode> m_pParent;
                std::shared_ptr<MeshNode> m_pParentOld;
                std::map<std::string, std::shared_ptr<MeshNode>> m_mChilds;
            };
        }
    }
}