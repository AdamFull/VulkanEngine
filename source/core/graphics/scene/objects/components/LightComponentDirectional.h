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
                alignas(8) std::array<float, SHADOW_MAP_CASCADE_COUNT> cascadeSplits;
                alignas(16) std::array<glm::mat4, SHADOW_MAP_CASCADE_COUNT> cascadeViewProjMat;
                alignas(16) glm::vec3 direction;
                alignas(16) glm::vec3 color;
                alignas(4) float intencity;
            };

            struct FFrustum
            {
                float neard{0.0f};
                float fard{0.0f};
                float fov{45.0/57.2957795 + 0.2f};
                float ratio{1.f};
                std::array<glm::vec3, 8> point;
            };

            class CLightComponentDirectional : public CLightComponent
            {
            public:
                friend class CLightSourceManager;
                void create() override;
                void update(float fDeltaTime) override;
            private:
                glm::mat4 getLightSpaceMatrixEx(const float lastSplitDist, const float splitDist);
                std::array<glm::vec3, 8> getFrostumCornersWorldSpaceEx(const glm::mat4& proj, const glm::mat4& view, float splitDist, float lastSplitDist);
                void updateCascadesEx();
                void updateCascades();
                std::array<FFrustum, SHADOW_MAP_CASCADE_COUNT> aFrustum;
            };
        }
    }
}