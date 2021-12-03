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
                class RenderObject : public DynamicNode<RenderObject>
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
                };
            }
        }
    }
}