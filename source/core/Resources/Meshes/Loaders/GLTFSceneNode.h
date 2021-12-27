#pragma once

namespace Engine
{
    namespace Resources
    {
        namespace Mesh { class MeshFragment; }
        namespace Loaders
        {
            class GLTFSceneNode;
            struct Skin
            {
                std::string name;
                std::shared_ptr<GLTFSceneNode> skeletonRoot = nullptr;
                std::vector<glm::mat4> inverseBindMatrices;
                std::vector<std::shared_ptr<GLTFSceneNode>> joints;
            };

            class GLTFSceneNode :public std::enable_shared_from_this<GLTFSceneNode>
            {
            public:
                // Deep search
                std::shared_ptr<GLTFSceneNode> Find(std::string srName);
                void AddChild(std::shared_ptr<GLTFSceneNode> child);
                void SetParent(std::shared_ptr<GLTFSceneNode> parent);
                void Attach(std::shared_ptr<GLTFSceneNode> child);
                void Detach(std::shared_ptr<GLTFSceneNode> child);

                std::string &GetName();
                std::string &GetUUID();
                std::shared_ptr<GLTFSceneNode> &GetParent();
                std::map<std::string, std::shared_ptr<GLTFSceneNode>> &GetChilds();

                FTransform GetTransform();
                const glm::vec3 GetPosition() const;
                const glm::vec3 GetRotation() const;
                const glm::vec3 GetScale() const;

                void SetName(std::string srName);

                void SetTransform(FTransform transformNew);
                void SetPosition(glm::vec3 position);
                void SetRotation(glm::vec3 rotation);
                void SetScale(glm::vec3 scale);

                uint32_t m_index;
		
                std::shared_ptr<Mesh::MeshFragment> m_pMesh;
                std::unique_ptr<Skin> m_pSkin;
            protected:
                std::string m_srName;
                std::string m_srUUID;
                FTransform m_transform;

                std::shared_ptr<GLTFSceneNode> m_pParent;
                std::shared_ptr<GLTFSceneNode> m_pParentOld;
                std::map<std::string, std::shared_ptr<GLTFSceneNode>> m_mChilds;
                std::map<std::string, std::string> m_mUUID;
            };
        }
    }
}