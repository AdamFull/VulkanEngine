#pragma once
#include "core/Objects/RenderObject.h"

namespace Engine
{
    class Device;

    class SceneRootComponent : public RenderObject
    {
    public:
        explicit SceneRootComponent(std::string srName = "SceneRoot") 
        {
            m_srName = srName;
        }
        
        void Create() override;
        void Render( vk::CommandBuffer& commandBuffer, uint32_t imageIndex) override;
        void Update(float fDeltaTime) override;
    };
}