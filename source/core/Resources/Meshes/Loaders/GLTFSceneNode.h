#pragma once
#include "util/Transform.hpp"

namespace engine
{
    namespace resources
    {
        namespace mesh { class CMeshFragment; }
        namespace loaders
        {
            class GLTFSceneNode;
            /**
             * @brief Skinning mesh data
             * 
             */
            struct FSkin
            {
                std::string name;
                ref_ptr<GLTFSceneNode> skeletonRoot = nullptr;
                std::vector<glm::mat4> inverseBindMatrices;
                std::vector<ref_ptr<GLTFSceneNode>> joints;
            };

            /**
             * @brief GLTF mesh scene node abstraction
             * 
             */
            class GLTFSceneNode :public std::enable_shared_from_this<GLTFSceneNode>
            {
            public:
                /**
                 * @brief Find node in scene tree
                 * 
                 * @param srName Scene node name
                 * @return ref_ptr<GLTFSceneNode> Found scene node smart pointer
                 */
                ref_ptr<GLTFSceneNode> find(const std::string& srName);

                /**
                 * @brief Add node child
                 * 
                 * @param child New node child
                 */
                void addChild(ref_ptr<GLTFSceneNode> child);

                /**
                 * @brief Set the Parent object
                 * 
                 * @param parent New node parent
                 */
                void setParent(ref_ptr<GLTFSceneNode> parent);

                /**
                 * @brief Attach child method. Works with addChild method
                 * 
                 * @param child New child
                 */
                void attach(ref_ptr<GLTFSceneNode> child);

                /**
                 * @brief Removes child and forgets his parent
                 * 
                 * @param child Detaching child object
                 */
                void detach(ref_ptr<GLTFSceneNode> child);

                /**
                 * @brief Get node name
                 * 
                 * @return std::string& Name reference
                 */
                std::string &getName();

                /**
                 * @brief Get node UUIDv4
                 * 
                 * @return std::string& UUID reference
                 */
                std::string &getUUID();

                /**
                 * @brief Get node parent
                 * 
                 * @return ref_ptr<GLTFSceneNode>& Node parent spart pointer object
                 */
                ref_ptr<GLTFSceneNode> &getParent();

                /**
                 * @brief Get node childs
                 * 
                 * @return std::map<std::string, ref_ptr<GLTFSceneNode>>& The map reference of node childs
                 */
                std::map<std::string, ref_ptr<GLTFSceneNode>> &getChilds();

                /**
                 * @brief Get node relative Transform object
                 * 
                 * @return FTransform Node relative transformation
                 */
                FTransform getTransform();

                /**
                 * @brief Get node relative position
                 * 
                 * @return const glm::vec3 Relative position point
                 */
                const glm::vec3 getPosition() const;

                /**
                 * @brief Get node relative rotation
                 * 
                 * @return const glm::vec3 Relative rotation vector
                 */
                const glm::vec3 getRotation() const;

                /**
                 * @brief Get relative node scale
                 * 
                 * @return const glm::vec3 Relative node scale vector
                 */
                const glm::vec3 getScale() const;

                /**
                 * @brief Set new node name
                 * 
                 * @param srName New name
                 */
                void setName(const std::string& srName);

                /**
                 * @brief Set new node local transform
                 * 
                 * @param transformNew New local transform
                 */
                void setTransform(FTransform transformNew);

                /**
                 * @brief Set new node local position
                 * 
                 * @param position New local position
                 */
                void setPosition(glm::vec3 position);

                /**
                 * @brief Set new node local rotation
                 * 
                 * @param rotation New local rotation
                 */
                void setRotation(glm::vec3 rotation);

                /**
                 * @brief Set new node local scale
                 * 
                 * @param scale New local scale
                 */
                void setScale(glm::vec3 scale);

                uint32_t m_index;
		
                ref_ptr<mesh::CMeshFragment> m_pMesh;
                scope_ptr<FSkin> m_pSkin;
            protected:
                std::string m_srName;
                std::string m_srUUID;
                FTransform m_transform;

                ref_ptr<GLTFSceneNode> m_pParent;
                ref_ptr<GLTFSceneNode> m_pParentOld;
                std::map<std::string, ref_ptr<GLTFSceneNode>> m_mChilds;
                std::map<std::string, std::string> m_mUUID;
            };
        }
    }
}