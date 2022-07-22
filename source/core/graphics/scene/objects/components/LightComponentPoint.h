#pragma once 
#include "LightComponent.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            struct FPointLight
            {
                alignas(16) glm::vec3 position;
                alignas(16) glm::vec3 color;
                alignas(4) float intencity;
                alignas(4) float radius;
            };

            class CLightComponentPoint : public CLightComponent
            {
            public:
                friend class CLightSourceManager;
                void create() override;
                void update(float fDeltaTime) override;
            };
        }
    }
}