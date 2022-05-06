#pragma once
#include "graphics/scene/objects/RenderObject.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            class CSceneRootComponent : public CRenderObject
            {
            public:
                explicit CSceneRootComponent(std::string srName = "SceneRoot")
                {
                    m_srName = srName;
                }

                void create() override;
                void render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;
                void update(float fDeltaTime) override;
            };
        }
    }
}