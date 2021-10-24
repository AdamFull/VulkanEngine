#pragma once
#include "Objects/RenderObject.h"

namespace Engine
{
    class ResourceManager;
    class ComponentBase : public RenderObject
    {
    public:
        void Create(std::shared_ptr<ResourceManager> resourceMgr) override;
        void ReCreate() override;
        void Update(float fDeltaTime) override;
        void Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;
    };
}