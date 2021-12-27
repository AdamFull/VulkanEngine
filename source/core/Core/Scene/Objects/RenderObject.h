#pragma once
#include "Core/VulkanSwapChain.h"
#include "Core/VulkanUniform.h"
#include "SceneConstruct.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;
    }
    namespace Core
    {
        namespace Scene
        {
            namespace Objects
            {
                class RenderObject : public std::enable_shared_from_this<RenderObject>
                {
                public:
                    RenderObject();
                    RenderObject(std::string srName);

                    // Create render object
                    virtual void Create(std::shared_ptr<Resources::ResourceManager> resourceMgr);
                    // Recreate render object
                    virtual void ReCreate();
                    // Do render things
                    virtual void Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex);
                    // Update object
                    virtual void Update(float fDeltaTime);
                    // Cleanup object( not deleting)
                    virtual void Cleanup();
                    // Destroy objects( deleting );
                    virtual void Destroy();

                    // Deep search
                    std::shared_ptr<RenderObject> Find(std::string srName);
                    void AddChild(std::shared_ptr<RenderObject> child);
                    void SetParent(std::shared_ptr<RenderObject> parent);
                    void Attach(std::shared_ptr<RenderObject> child);
                    void Detach(std::shared_ptr<RenderObject> child);

                    std::string &GetName();
                    //std::string &GetUUID();
                    std::shared_ptr<RenderObject> &GetParent();
                    std::unordered_map<std::string, std::shared_ptr<RenderObject>> &GetChilds();

                    FTransform GetTransform();
                    const glm::vec3 GetPosition() const;
                    const glm::vec3 GetRotation() const;
                    const glm::vec3 GetScale() const;

                    void SetName(std::string srName);

                    void SetTransform(FTransform transformNew);
                    void SetPosition(glm::vec3 position);
                    void SetRotation(glm::vec3 rotation);
                    void SetScale(glm::vec3 scale);

                protected:
                    std::string m_srName;
                    //std::string m_srUUID;
                    FTransform m_transform;

                    std::shared_ptr<RenderObject> m_pParent;
                    std::shared_ptr<RenderObject> m_pParentOld;
                    std::unordered_map<std::string, std::shared_ptr<RenderObject>> m_mChilds;
                    //std::unordered_map<std::string, std::string> m_mUUID;
                };
            }
        }
    }
}