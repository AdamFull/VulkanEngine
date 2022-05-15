#pragma once
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/buffer/VulkanUniform.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            class CLightSourceBase : public CRenderObject
            {
            public:
                CLightSourceBase();

                glm::vec3 getColor() const { return vColor; }
                inline void setColor(glm::vec3 color) { vColor = color; }
                float getIntencity() { return fIntencity; }
                inline void setIntencity(float intencity) { fIntencity = intencity; }

                Resources::ELightSourceType getLightType() { return eLightType; }
            protected:
                Resources::ELightSourceType eLightType;
                glm::vec3 vColor{};
                float fIntencity{1.f};
            };
        }
    }
}