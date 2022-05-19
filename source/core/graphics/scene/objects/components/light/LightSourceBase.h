#pragma once
#include "graphics/scene/objects/RenderObject.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            struct FPointLight
            {
                glm::vec3 position;
                glm::vec3 color;
                float radius;
                float intencity;
            };

            struct FDirectionalLight
            {
                glm::vec3 color;
                glm::vec3 direction;
                float intencity;
            };

            struct FSpotLight
            {
                glm::vec3 position;
                glm::vec3 color;
                glm::vec3 direction;
                float intencity;
                float cutoff;
            };

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