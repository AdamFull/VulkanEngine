#pragma once
#include "SceneComponent.h"
#include "resources/ResourceCunstruct.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CLightComponent : public CSceneComponent
            {
            public:
                friend class CLightSourceManager;
                virtual void create() override;
                virtual void update(float fDeltaTime) override;

                void setType(resources::ELightSourceType type);
                resources::ELightSourceType getType();

                const glm::vec3 getPosition();
                const glm::vec3 getDirection();

                void setColor(const glm::vec3& color);
                const glm::vec3& getColor();

                void setInnerAngle(float angle);
                const float& getInnerAngle();

                void setOuterAngle(float angle);
                const float& getOuterAngle();

                void setIntencity(float intencity);
                const float& getIntencity();

                void setRadius(float radius);
                const float& getRadius();

                const glm::mat4& getShadowView();
                const std::array<glm::mat4, 6>& getShadowViews();
                const std::array<float, SHADOW_MAP_CASCADE_COUNT>& getCascadeSplits();
                const std::array<glm::mat4, SHADOW_MAP_CASCADE_COUNT>& getCascadeViews();
            protected:
                resources::ELightSourceType eType;
                glm::vec3 color;
                float innerConeAngle{0.1}, outerConeAngle{0.78}, intencity{1.0f}, radius{1.0f};
                glm::mat4 shadowView{};
                std::array<glm::mat4, 6> shadowViews;
                std::array<float, SHADOW_MAP_CASCADE_COUNT> aCascadeSplits;
                std::array<glm::mat4, SHADOW_MAP_CASCADE_COUNT> aCascadeViewProjMat;
            };
        }
    }
}