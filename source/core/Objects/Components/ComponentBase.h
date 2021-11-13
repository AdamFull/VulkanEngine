#pragma once
#include "Objects/RenderObject.h"

namespace Engine
{
    namespace Objects
    {
        namespace Components
        {
            class ComponentBase : public RenderObject
            {
            public:
                void Create(std::shared_ptr<Resources::ResourceManager> resourceMgr) override;
                void ReCreate() override;
                void Update(float fDeltaTime) override;
                void Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
                void Destroy() override;
            };
        }
    }
}