#pragma once
#include "Core/Scene/Objects/Transform.h"

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

            class GLTFSceneNode : public std::enable_shared_from_this<GLTFSceneNode>
            {
            public:
                Core::Scene::Objects::FTransform GetTransform();
                glm::vec3 GetPosition();
                glm::vec3 GetRotation();
                glm::vec3 GetScale();

                void SetParent(std::shared_ptr<GLTFSceneNode> parent);
                void Attach(std::shared_ptr<GLTFSceneNode> child);
                void Detach(std::shared_ptr<GLTFSceneNode> child);

                std::shared_ptr<GLTFSceneNode> Find(uint32_t index);

                inline std::shared_ptr<GLTFSceneNode> GetParent() { return m_pParent; }
                inline std::map<std::string, std::shared_ptr<GLTFSceneNode>> &GetChilds() { return m_mChilds; }
                void AddChild(std::shared_ptr<GLTFSceneNode> child);

                std::string m_srName;
                Core::Scene::Objects::FTransform m_transform;
                uint32_t m_index;
		
                std::shared_ptr<Mesh::MeshFragment> m_pMesh;
                std::unique_ptr<Skin> m_pSkin;
            private:
                std::shared_ptr<GLTFSceneNode> m_pParent;
                std::shared_ptr<GLTFSceneNode> m_pParentOld;
                std::map<std::string, std::shared_ptr<GLTFSceneNode>> m_mChilds;
            };
        }
    }
}