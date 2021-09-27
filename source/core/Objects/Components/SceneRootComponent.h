#pragma once
#include "core/Objects/RenderObject.h"

namespace Engine
{
    class Device;

    class SceneRootComponent : public RenderObject
    {
    public:
        void Create(std::unique_ptr<Device>& device) override;
        void Render(float fDeltaTime, vk::CommandBuffer& commandBuffer) override;
        void Update(float fDeltaTime) override;
    };
}