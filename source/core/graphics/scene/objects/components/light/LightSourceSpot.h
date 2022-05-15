#pragma once
#include "LightSourceBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            class CLightSourceSpot : public CLightSourceBase
            {
            public:
                CLightSourceSpot() = default;
                FSpotLight getLight();

                void setDirection(const glm::vec3& value) { direction = value; }
                const glm::vec3 getDirection() const { return direction; }

                void setCutoff(float value) { cutoff = value; }
                const float getCutoff() const { return cutoff; }
            protected:
                glm::vec3 direction{1.0};
                float cutoff{2.f};
            };
        }
    }
}