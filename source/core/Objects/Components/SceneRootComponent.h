#pragma once
#include "Objects/RenderObject.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;
    }
    namespace Objects
    {
        namespace Components
        {
            class SceneRootComponent : public RenderObject
            {
            public:
                explicit SceneRootComponent(std::string srName = "SceneRoot")
                {
                    m_srName = srName;
                }

                void Create(std::shared_ptr<Resources::ResourceManager> resourceMgr) override;
                void Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;
                void Update(float fDeltaTime) override;
            };
        }
    }
}