#pragma once
#include "Objects/RenderObject.h"

namespace Engine
{
    class Device;
    class ResourceManager;

    class SceneRootComponent : public RenderObject
    {
    public:
        explicit SceneRootComponent(std::string srName = "SceneRoot") 
        {
            m_srName = srName;
        }
        
        void Create(std::shared_ptr<ResourceManager> resourceMgr) override;
        void Render( vk::CommandBuffer& commandBuffer, uint32_t imageIndex) override;
        void Update(float fDeltaTime) override;
    };
}