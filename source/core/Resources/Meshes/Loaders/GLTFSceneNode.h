#pragma once
#include "util/Transform.hpp"

namespace Engine
{
    namespace Resources
    {
        namespace Mesh { class CMeshFragment; }
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
                std::shared_ptr<GLTFSceneNode> find(std::string srName);
                void addChild(std::shared_ptr<GLTFSceneNode> child);
                void setParent(std::shared_ptr<GLTFSceneNode> parent);
                void attach(std::shared_ptr<GLTFSceneNode> child);
                void detach(std::shared_ptr<GLTFSceneNode> child);

                std::string &getName();
                std::string &getUUID();
                std::shared_ptr<GLTFSceneNode> &getParent();
                std::map<std::string, std::shared_ptr<GLTFSceneNode>> &getChilds();

                FTransform getTransform();
                const glm::vec3 getPosition() const;
                const glm::vec3 getRotation() const;
                const glm::vec3 getScale() const;

                void setName(std::string srName);

                void setTransform(FTransform transformNew);
                void setPosition(glm::vec3 position);
                void setRotation(glm::vec3 rotation);
                void setScale(glm::vec3 scale);

                uint32_t m_index;
		
                std::shared_ptr<Mesh::CMeshFragment> m_pMesh;
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