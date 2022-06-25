#pragma once 
#include "LightComponent.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            struct FDirectionalLight
            {
                alignas(16)glm::vec3 direction;
                alignas(16) glm::vec3 color;
                alignas(4) float intencity;
                std::array<float, SHADOW_MAP_CASCADE_COUNT> cascadeSplits;
                std::array<glm::mat4, SHADOW_MAP_CASCADE_COUNT> cascadeViewProjMat;
            };

            class CLightComponentDirectional : public CLightComponent
            {
            public:
                friend class CLightSourceManager;
                void create() override;
                void update(float fDeltaTime) override;
            private:
                void updateCascades();
            };
        }
    }
}