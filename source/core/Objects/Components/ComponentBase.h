#pragma once
#include "Objects/RenderObject.h"

namespace Engine
{
    class ResourceManager;
    class ComponentBase : public RenderObject
    {
    public:
        void Create() override;
        void ReCreate() override;
        void Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex) override;
        void Update(float fDeltaTime) override;
        void Cleanup() override;
        void Destroy() override;
    protected:
        std::shared_ptr<ResourceManager> pResourceManager;
    };
}