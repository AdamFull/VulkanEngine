#pragma once
#include "SceneComponent.h"
#include "resources/ResourceCunstruct.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            struct FLight
            {
                alignas(16) glm::vec3 position{0.0};
                alignas(16) glm::vec3 direction{0.0};
                alignas(16) glm::vec3 color{0.0};
                alignas(4) float radius{0.0};
                alignas(4) float intencity{0.0};
                alignas(4) int type{0};
            };

            class CLightComponent : public CSceneComponent
            {
            public:
                friend class CLightSourceManager;
                void create() override {}
                void reCreate() override {}
                void render(vk::CommandBuffer &commandBuffer) override {}
                void update(float fDeltaTime) override {}
                void cleanup() override {}
                void destroy() override {}

                void setType(resources::ELightSourceType type);
                resources::ELightSourceType getType();

                void setRadius(float radius);
                const float getRadius();

                void setColor(const glm::vec3& color);
                const glm::vec3& getColor();

                void setIntencity(float intencity);
                const float getIntencity();

                FLight& getLight();
            protected:
                FLight lightData{};
            };
        }
    }
}