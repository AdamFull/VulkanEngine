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
                alignas(16) glm::vec3 position;
                alignas(16) glm::vec3 color;
                alignas(4) float radius;
                alignas(4) float intencity;
            };

            struct FDirectionalLight
            {
                alignas(16) glm::vec3 color;
                alignas(16) glm::vec3 direction;
                alignas(4) float intencity;
            };

            struct FSpotLight
            {
                alignas(16) glm::vec3 position;
                alignas(16) glm::vec3 color;
                alignas(16) glm::vec3 direction;
                alignas(4) float intencity;
                alignas(4) float cutoff;
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