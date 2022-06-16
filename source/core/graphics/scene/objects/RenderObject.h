#pragma once
#include "util/Transform.hpp"
#include "graphics/scene/objects/components/MeshComponent.h"
#include "graphics/scene/objects/components/CameraComponent.h"
#include "graphics/scene/objects/components/LightComponent.h"

namespace engine
{
    namespace core
    {
        namespace scene
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
                virtual void render(vk::CommandBuffer &commandBuffer);
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

                // Deep search
                ref_ptr<CRenderObject>& find(std::string name);
                ref_ptr<CRenderObject>& find(uint64_t id);
                void addChild(ref_ptr<CRenderObject> &child);
                void setParent(ref_ptr<CRenderObject> parent);
                void attach(ref_ptr<CRenderObject> &&child);
                void detach(ref_ptr<CRenderObject> child);

                uint64_t getId() const { return objectId; }
                std::string &getName();
                // std::string &GetUUID();
                ref_ptr<CRenderObject>& getParent();
                std::unordered_map<std::string, ref_ptr<CRenderObject>> &getChilds();

                FTransform getTransform();
                FTransform& getLocalTransform() { return transform; }
                const glm::vec3 getPosition() const;
                const glm::vec3 getLocalPosition() const { return transform.getPosition(); }
                const glm::vec3 getRotation() const;
                const glm::vec3 getLocalRotation() const { return transform.getRotation(); }
                const glm::vec3 getScale() const;
                const glm::vec3 getLocalScale() const { return transform.getScale(); }

                void setName(std::string name);

                void setTransform(FTransform transformNew);
                void setPosition(glm::vec3 position);
                void setRotation(glm::vec3 rotation);
                void setScale(glm::vec3 scale);

                void setIndex(uint32_t index) { objectIndex = index; };
                uint32_t getIndex() { return objectIndex; }

                void setMesh(ref_ptr<CMeshComponent>&& component) 
                { 
                    pMesh = std::move(component);
                    pMesh->setParent(shared_from_this());
                }

                void setCamera(ref_ptr<CCameraComponent>&& component) 
                { 
                    pCamera = std::move(component);
                    pCamera->setParent(shared_from_this());
                }

                void setLight(ref_ptr<CLightComponent>&& component) 
                { 
                    pLight = std::move(component);
                    pLight->setParent(shared_from_this());
                }

                ref_ptr<CMeshComponent>& getMesh() { return pMesh; }
                ref_ptr<CCameraComponent>& getCamera() { return pCamera; }
                ref_ptr<CLightComponent>& getLight() { return pLight; }

            protected:
                uint64_t objectId{0};
                uint32_t objectIndex{0};
                std::string srName;
                FTransform transform;
                bool bVisible{true};
                bool bEnable{true};

                ref_ptr<CMeshComponent> pMesh;
                ref_ptr<CCameraComponent> pCamera;
                ref_ptr<CLightComponent> pLight;

                ref_ptr<CRenderObject> pNull{nullptr};
                ref_ptr<CRenderObject> pParent;
                ref_ptr<CRenderObject> pParentOld;
                std::unordered_map<std::string, ref_ptr<CRenderObject>> mChilds;
            };
        }
    }
}