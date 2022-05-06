#pragma once
#include "graphics/scene/objects/RenderObject.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            class CComponentBase : public CRenderObject
            {
            public:
                void create() override;
                void reCreate() override;
                void update(float fDeltaTime) override;
                void render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;
                void cleanup() override;
                void destroy() override;
            };
        }
    }
}