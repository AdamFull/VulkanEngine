#pragma once
#include "Objects/Transform.h"

namespace Engine
{
    namespace Resources
    {
        namespace Mesh
        {
            class MeshBase;
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
                void Create();
                void ReCreate();
                void Update(uint32_t imageIndex);
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
                void Cleanup();
                void Destroy();

                Objects::FTransform GetTransform();
                glm::vec3 GetPosition();
                glm::vec3 GetRotation();
                glm::vec3 GetScale();

                inline void SetTransform(FTransform transformNew) { m_transform = transformNew; }
                inline void SetPosition(glm::vec3 position) { m_transform.pos = position; }
                inline void SetRotation(glm::vec3 rotation) { m_transform.rot = rotation; }
                inline void SetScale(glm::vec3 scale) { m_transform.scale = scale; }
                void SetLocalMatrix(glm::mat4&& matrix);
                glm::mat4 GetLocalMatrix();
                glm::mat4 GetMatrix();

                inline void SetName(std::string srName) { m_srName = srName; }
                inline std::string &GetName() { return m_srName; }

                void SetParent(std::shared_ptr<GLTFSceneNode> parent);
                void Attach(std::shared_ptr<GLTFSceneNode> child);
                void Detach(std::shared_ptr<GLTFSceneNode> child);

                inline std::shared_ptr<GLTFSceneNode> GetParent() { return m_pParent; }
                inline std::map<std::string, std::shared_ptr<GLTFSceneNode>> &GetChilds() { return m_mChilds; }
                void AddChild(std::shared_ptr<GLTFSceneNode> child);
            protected:
                std::string m_srName;
                Objects::FTransform m_transform;
                glm::mat4 m_mMatrix;
		
                std::shared_ptr<MeshBase> m_pMesh;
                std::unique_ptr<Skin> m_pSkin;
            private:
                std::shared_ptr<GLTFSceneNode> m_pParent;
                std::shared_ptr<GLTFSceneNode> m_pParentOld;
                std::map<std::string, std::shared_ptr<GLTFSceneNode>> m_mChilds;
            };
        }
    }
}