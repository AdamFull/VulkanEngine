#pragma once
#include "Renderer/VulkanSwapChain.h"
#include "Renderer/VulkanUniform.h"

namespace Engine
{
    class Device;

    struct FTransform
    {
        glm::vec3 pos{};
        glm::vec3 rot{};
        glm::vec3 scale{1.f, 1.f, 1.f};

        glm::mat4 GetModel();
        glm::mat3 GetNormal();
    };

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
        virtual void Create();
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

        virtual std::shared_ptr<RenderObject> Find(std::string srName);

        virtual const glm::vec3 GetForwardVector();
        virtual const glm::vec3 GetRightVector();
        virtual const glm::vec3 GetUpVector();
        virtual inline FTransform GetTransform() { return m_transform; }
        virtual inline void SetTransform(FTransform transformNew) { m_transform = transformNew; }

        virtual void SetParent(std::shared_ptr<RenderObject> parent);
        virtual void Attach(std::shared_ptr<RenderObject> child);
        virtual void Detach(std::shared_ptr<RenderObject> child);

        virtual inline std::string GetName() { return m_srName; }

        virtual inline std::shared_ptr<RenderObject>& GetParent() { return m_pParent; }
        virtual inline std::map<std::string, std::shared_ptr<RenderObject>>& GetChilds() { return m_mChilds; }
    protected:
        virtual void AddChild(std::shared_ptr<RenderObject> child);
        FTransform m_transform;
        std::string m_srName;
    private:
        std::shared_ptr<RenderObject> m_pParent;
        std::shared_ptr<RenderObject> m_pParentOld;
        std::map<std::string, std::shared_ptr<RenderObject>> m_mChilds;

        
    };
}