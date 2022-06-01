#pragma once
#include "LightSourceBase.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CLightSourcePoint : public CLightSourceBase
            {
            public:
                CLightSourcePoint();
                FPointLight getLight();

                void setAttenuation(float value) { fAttenuation = value; }
                const float getAttenuation() const { return fAttenuation; }

            protected:
                float fAttenuation{1.f};
            };
        }
    }
}