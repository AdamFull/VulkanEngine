#pragma once
#include "util/Transform.hpp"
#include "graphics/scene/SceneConstruct.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            enum class ERenderObjectType
            {
                eNone,
                eMesh,
                eCamera,
                eLight
            };

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

                void setVisible(bool value) { bVisible = value; }
                bool isVisible() const { return bVisible; }
                void setEnable(bool value) { bEnable = value; }
                bool isEnabled() const { return bEnable; }
                bool wasRendered() const { return bWasRendered; }

                // Deep search
                ref_ptr<CRenderObject> find(std::string name);
                ref_ptr<CRenderObject> find(uint64_t id);
                void addChild(ref_ptr<CRenderObject>& child);
                void setParent(ref_ptr<CRenderObject> parent);
                void attach(ref_ptr<CRenderObject>&& child);
                void detach(ref_ptr<CRenderObject> child);

                uint64_t getId() const { return objectId; }
                std::string &getName();
                // std::string &GetUUID();
                ref_ptr<CRenderObject>& getParent();
                std::unordered_map<std::string, ref_ptr<CRenderObject>> &getChilds();

                FTransform getTransform();
                FTransform getLocalTransform() { return transform; }
                const glm::vec3 getPosition() const;
                const glm::vec3 getLocalPosition() const { return transform.pos; }
                const glm::vec3 getRotation() const;
                const glm::vec3 getLocalRotation() const { return transform.rot; }
                const glm::vec3 getScale() const;
                const glm::vec3 getLocalScale() const { return transform.scale; }

                void setCullingRadius(float radius) { fCullingRadius = radius; }
                void setCullable(bool cullable) { bEnableCulling = cullable; }
                void setBounds(glm::vec3 begin, glm::vec3 end) { boundingBox.first = begin; boundingBox.second = end; }
                void setCyllingType(ECullingType type) { eCullingType = type; }

                const float getCullingRadius() const { return fCullingRadius; }
                const bool isCullable() const { return bEnableCulling; }
                const std::pair<glm::vec3, glm::vec3> getBounds() const { return boundingBox; } 
                const ECullingType getCullingType() const { return eCullingType; }
                void setCullingType(ECullingType eType) { eCullingType = eType; }

                void setName(std::string name);

                void setTransform(FTransform transformNew);
                void setPosition(glm::vec3 position);
                void setRotation(glm::vec3 rotation);
                void setScale(glm::vec3 scale);

                ERenderObjectType getObjectType() const { return eObjectType; }

            protected:
                uint64_t objectId{0};
                std::string srName;
                FTransform transform;
                bool bVisible{true};
                bool bEnable{true};
                bool bWasRendered{true};
                bool bEnableCulling{true};
                bool bHasInstances{false};
                float fCullingRadius{1.1f};
                std::pair<glm::vec3, glm::vec3> boundingBox{};
                ECullingType eCullingType{ECullingType::eBySphere};
                ERenderObjectType eObjectType{ERenderObjectType::eNone};

                ref_ptr<CRenderObject> pParent;
                ref_ptr<CRenderObject> pParentOld;
                std::unordered_map<std::string, ref_ptr<CRenderObject>> mChilds;
            };
        }
    }
}