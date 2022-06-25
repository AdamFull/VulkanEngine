#pragma once 
#include "LightComponent.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            struct FSpotLight
            {
                glm::mat4 shadowView;
                alignas(16) glm::vec3 position;
                alignas(16) glm::vec3 direction;
                alignas(16) glm::vec3 color;
                alignas(4) float intencity;
                alignas(4) float innerConeAngle;
                alignas(4) float outerConeAngle;
                alignas(4) bool toTarget{true};
            };

            class CLightComponentSpot : public CLightComponent
            {
            public:
                friend class CLightSourceManager;
                void create() override;
                void update(float fDeltaTime) override;
            };
        }
    }
}