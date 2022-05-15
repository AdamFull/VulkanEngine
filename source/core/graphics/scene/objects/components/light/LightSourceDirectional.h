#pragma once
#include "LightSourceBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            class CLightSourceDirectional : public CLightSourceBase
            {
            public:
                CLightSourceDirectional() = default;
                FDirectionalLight getLight();

                void setDirection(const glm::vec3& value) { transform.rot = value; }
                const glm::vec3 getDirection() const { return transform.rot; }
            protected:
                glm::vec3 direction{1.0};
            };
        }
    }
}