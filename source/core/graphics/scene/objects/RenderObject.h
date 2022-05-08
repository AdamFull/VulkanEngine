#pragma once
#include "util/Transform.hpp"
#include "graphics/buffer/VulkanUniform.h"
#include "graphics/scene/SceneConstruct.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            class CRenderObject : public std::enable_shared_from_this<CRenderObject>
            {
            public:
                CRenderObject();
                CRenderObject(std::string srName);
                ~CRenderObject();

                // Create render object
                virtual void create();
                // Recreate render object
                virtual void reCreate();
                // Do render things
                virtual void render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex);
                // Update object
                virtual void update(float fDeltaTime);
                // Cleanup object( not deleting)
                virtual void cleanup();
                // Destroy objects( deleting );
                virtual void destroy();

                // Deep search
                std::shared_ptr<CRenderObject> find(std::string srName);
                void addChild(std::shared_ptr<CRenderObject> child);
                void setParent(std::shared_ptr<CRenderObject> parent);
                void attach(std::shared_ptr<CRenderObject> child);
                void detach(std::shared_ptr<CRenderObject> child);

                std::string &getName();
                // std::string &GetUUID();
                std::shared_ptr<CRenderObject> &getParent();
                std::unordered_map<std::string, std::shared_ptr<CRenderObject>> &getChilds();

                FTransform getTransform();
                const glm::vec3 getPosition() const;
                const glm::vec3 getRotation() const;
                const glm::vec3 getScale() const;

                void setCullingRadius(float radius) { fCullingRadius = radius; }
                void setCullable(bool cullable) { bEnableCulling = cullable; }
                void setBounds(glm::vec3 begin, glm::vec3 end) { boundingBox.first = begin; boundingBox.second = end; }
                void setCyllingType(ECullingType type) { eCullingType = type; }

                const float getCullingRadius() const { return fCullingRadius; }
                const bool isCullable() const { return bEnableCulling; }
                const std::pair<glm::vec3, glm::vec3> getBounds() const { return boundingBox; } 
                const ECullingType getCullingType() const { return eCullingType; }

                void setName(std::string srName);

                void setTransform(FTransform transformNew);
                void setPosition(glm::vec3 position);
                void setRotation(glm::vec3 rotation);
                void setScale(glm::vec3 scale);

                bool checkFrustum(const std::shared_ptr<CRenderObject>& object);

            protected:
                std::string m_srName;
                // std::string m_srUUID;
                FTransform m_transform;
                bool bVisible{true};
                bool bEnableCulling{true};
                bool bHasInstances{false};
                float fCullingRadius{2.f};
                std::pair<glm::vec3, glm::vec3> boundingBox{};
                ECullingType eCullingType{ECullingType::eBySphere};

                std::shared_ptr<CRenderObject> m_pParent;
                std::shared_ptr<CRenderObject> m_pParentOld;
                std::unordered_map<std::string, std::shared_ptr<CRenderObject>> m_mChilds;
                // std::unordered_map<std::string, std::string> m_mUUID;
            };
        }
    }
}