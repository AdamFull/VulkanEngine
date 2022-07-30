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
            using component_type = std::variant<
                        utl::ref_ptr<CMeshComponent>, 
                        utl::ref_ptr<CCameraComponent>, 
                        utl::ref_ptr<CLightComponent>, 
                        utl::ref_ptr<CScriptingComponent>>;

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

                template<class _Ty>
                void addComponent(utl::ref_ptr<_Ty>& component)
                {
                    component->setParent(shared_from_this());
                    mComponents.emplace(typeid(_Ty).name(), component);
                }

                template<class _Ty>
                utl::weak_ptr<_Ty> getComponent()
                {
                    auto found = mComponents.find(typeid(_Ty).name());
                    if(found != mComponents.end())
                        return std::get<utl::ref_ptr<_Ty>>(found->second);
                    return utl::ref_ptr<_Ty>{nullptr};
                }

                std::map<std::string, component_type>& getComponents() { return mComponents; }

            protected:
                uint64_t objectId{0};
                uint32_t objectIndex{0};
                std::string srName;
                FTransform transform, transform_old;
                bool bVisible{true};
                bool bEnable{true};

                std::map<std::string, component_type> mComponents;

                utl::ref_ptr<CRenderObject> pNull{nullptr};
                utl::ref_ptr<CRenderObject> pParent;
                utl::ref_ptr<CRenderObject> pParentOld;
                std::unordered_map<std::string, utl::ref_ptr<CRenderObject>> mChilds;
            };
        }
    }
}