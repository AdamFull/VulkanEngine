#pragma once
#include "Renderer/VulkanSwapChain.h"
#include "Renderer/VulkanUniform.h"
#include "Transform.h"
#include "SceneConstruct.h"

namespace Engine
{
    class Device;
    class ResourceManager;

    class RenderObject : public std::enable_shared_from_this<RenderObject>
    {
    public:
        RenderObject() = default;
        explicit RenderObject(std::string srName)
        {
            m_srName = srName;
        }

        virtual ~RenderObject() 
        {
            
        }

        //Create render object
        virtual void Create(std::shared_ptr<ResourceManager> resourceMgr);
        //Recreate render object
        virtual void ReCreate();
        //Do render things
        virtual void Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex);
        //Update object
        virtual void Update(float fDeltaTime);
        //Cleanup object( not deleting)
        virtual void Cleanup();
        //Destroy objects( deleting );
        virtual void Destroy();

        virtual inline void SetName(std::string srName) { m_srName = srName; }

        virtual std::shared_ptr<RenderObject> Find(std::string srName);

        virtual const glm::vec3 GetForwardVector();
        virtual const glm::vec3 GetRightVector();
        virtual const glm::vec3 GetUpVector();
        virtual FTransform GetTransform();
        virtual glm::vec3 GetPosition();
        virtual glm::vec3 GetRotation();
        virtual glm::vec3 GetScale();

        virtual inline void SetTransform(FTransform transformNew) { m_transform = transformNew; }
        virtual inline void SetPosition(glm::vec3 position) { m_transform.pos = position; }
        virtual inline void SetRotation(glm::vec3 rotation) { m_transform.rot = rotation; }
        virtual inline void SetScale(glm::vec3 scale) { m_transform.scale = scale; }

        virtual void SetParent(std::shared_ptr<RenderObject> parent);
        virtual void Attach(std::shared_ptr<RenderObject> child);
        virtual void Detach(std::shared_ptr<RenderObject> child);

        virtual inline std::string GetName() { return m_srName; }

        virtual inline std::shared_ptr<RenderObject>& GetParent() { return m_pParent; }
        virtual inline std::map<std::string, std::shared_ptr<RenderObject>>& GetChilds() { return m_mChilds; }
        virtual void AddChild(std::shared_ptr<RenderObject> child);
        
    protected:
        FTransform m_transform;
        std::string m_srName;
    private:
        std::shared_ptr<RenderObject> m_pParent;
        std::shared_ptr<RenderObject> m_pParentOld;
        std::map<std::string, std::shared_ptr<RenderObject>> m_mChilds;
    };
}