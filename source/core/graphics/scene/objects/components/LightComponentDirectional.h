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

            class CLightComponentDirectional : public CLightComponent
            {
            public:
                friend class CLightSourceManager;
                void create() override;
                void update(float fDeltaTime) override;
            private:
                glm::mat4 getLightSpaceMatrixEx(const float lastSplitDist, const float splitDist);
                std::array<glm::vec3, 8> getFrostumCornersWorldSpaceEx(const glm::mat4& proj, const glm::mat4& view, float splitDist, float lastSplitDist);
                void updateCascades();
            };
        }
    }
}