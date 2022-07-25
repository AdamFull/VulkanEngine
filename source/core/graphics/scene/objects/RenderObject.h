#pragma once
#include "util/Transform.hpp"
#include "graphics/scene/objects/components/MeshComponent.h"
#include "graphics/scene/objects/components/CameraComponent.h"
#include "graphics/scene/objects/components/LightComponent.h"
#include "graphics/scene/objects/components/ScriptingComponent.h"

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
                CRenderObject(const std::string& srName);
                ~CRenderObject();

                // Create render object
                virtual void create();
                // Recreate render object
                virtual void reCreate();
                // Do render things
                virtual void render(vk::CommandBuffer &commandBuffer);
                // Update object
                virtual void update(float fDeltaTime);

                void setVisible(bool value) { bVisible = value; }
                bool isVisible() const { return bVisible; }
                void setEnable(bool value) { bEnable = value; }
                bool isEnabled() const { return bEnable; }

                // Deep search
                utl::ref_ptr<CRenderObject>& find(const std::string& name);
                utl::ref_ptr<CRenderObject>& find(uint64_t id);
                void addChild(utl::ref_ptr<CRenderObject> &child);
                void setParent(utl::ref_ptr<CRenderObject> parent);
                void attach(utl::ref_ptr<CRenderObject> &&child);
                void detach(utl::ref_ptr<CRenderObject> child);

                uint64_t getId() const { return objectId; }
                std::string &getName();
                // std::string &GetUUID();
                utl::ref_ptr<CRenderObject>& getParent();
                std::unordered_map<std::string, utl::ref_ptr<CRenderObject>> &getChilds();

                FTransform getTransform();
                FTransform& getLocalTransform() { return transform; }
                glm::mat4 getModel();
                glm::mat4 getModelOld();
                glm::vec3 getPosition();
                const glm::vec3 getLocalPosition() const { return transform.getPosition(); }
                glm::vec3 getRotation();
                const glm::vec3 getLocalRotation() const { return transform.getRotation(); }
                glm::vec3 getScale();
                const glm::vec3 getLocalScale() const { return transform.getScale(); }

                void setName(const std::string& name);

                void setTransform(FTransform transformNew);
                void setPosition(glm::vec3 position);
                void setRotation(glm::vec3 rotation);
                void setScale(glm::vec3 scale);

                void setIndex(uint32_t index) { objectIndex = index; };
                uint32_t getIndex() { return objectIndex; }

                void setMesh(utl::ref_ptr<CMeshComponent>&& component) 
                { 
                    pMesh = std::move(component);
                    pMesh->setParent(shared_from_this());
                }

                void setCamera(utl::ref_ptr<CCameraComponent>&& component) 
                { 
                    pCamera = std::move(component);
                    pCamera->setParent(shared_from_this());
                }

                void setLight(utl::ref_ptr<CLightComponent>&& component) 
                { 
                    pLight = std::move(component);
                    pLight->setParent(shared_from_this());
                }

                void setScript(utl::ref_ptr<CScriptingComponent>&& component) 
                { 
                    pScript = std::move(component);
                    pScript->setParent(shared_from_this());
                }

                utl::ref_ptr<CMeshComponent>& getMesh() { return pMesh; }
                utl::ref_ptr<CCameraComponent>& getCamera() { return pCamera; }
                utl::ref_ptr<CLightComponent>& getLight() { return pLight; }
                utl::ref_ptr<CScriptingComponent>& getScript() { return pScript; }

            protected:
                uint64_t objectId{0};
                uint32_t objectIndex{0};
                std::string srName;
                FTransform transform;
                bool bVisible{true};
                bool bEnable{true};

                utl::ref_ptr<CMeshComponent> pMesh;
                utl::ref_ptr<CCameraComponent> pCamera;
                utl::ref_ptr<CLightComponent> pLight;
                utl::ref_ptr<CScriptingComponent> pScript;

                utl::ref_ptr<CRenderObject> pNull{nullptr};
                utl::ref_ptr<CRenderObject> pParent;
                utl::ref_ptr<CRenderObject> pParentOld;
                std::unordered_map<std::string, utl::ref_ptr<CRenderObject>> mChilds;
            };
        }
    }
}