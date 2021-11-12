#pragma once
#include "MeshComponentBase.h"

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
            class MeshComponentBase;
            class MeshVolumeComponent : public MeshComponentBase
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