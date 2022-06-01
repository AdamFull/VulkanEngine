#pragma once
#include "resources/ResourceManager.h"
#include "graphics/image/Image.h"
#include "graphics/scene/objects/RenderObject.h"

namespace engine
{
    namespace resources
    {
        namespace material{ class CMaterialBase; }
    }
    namespace core
    {
        namespace render
        {
            class CSubpass
            {
            public:
                CSubpass() = default;

                virtual void create() {}
                virtual void reCreate() {}
                virtual void render(vk::CommandBuffer& commandBuffer) {}
                virtual void cleanup();
            protected:
                ref_ptr<resources::material::CMaterialBase> pMaterial;
            };
        }
    }
}