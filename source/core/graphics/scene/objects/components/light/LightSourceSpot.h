#pragma once
#include "LightSourceBase.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CLightSourceSpot : public CLightSourceBase
            {
            public:
                CLightSourceSpot();
                FSpotLight getLight();

                void setDirection(const glm::vec3 &value) { transform.rot = value; }
                const glm::vec3 getDirection() const { return transform.rot; }

                void setCutoff(float value) { cutoff = value; }
                const float getCutoff() const { return cutoff; }

            protected:
                glm::vec3 direction{1.0};
                float cutoff{0.f};
            };
        }
    }
}