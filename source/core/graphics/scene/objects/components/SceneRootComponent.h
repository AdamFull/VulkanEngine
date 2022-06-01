#pragma once
#include "graphics/scene/objects/RenderObject.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CSceneRootComponent : public CRenderObject
            {
            public:
                explicit CSceneRootComponent(std::string name = "SceneRoot")
                {
                    srName = name;
                }

                void create() override;
                void render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;
                void update(float fDeltaTime) override;
            };
        }
    }
}