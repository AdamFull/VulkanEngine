#pragma once
#include "SceneComponent.h"
#include "frustum.hpp"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CCameraComponent : public CSceneComponent
            {
            public:
                void create() override;
                void reCreate() override {}
                void render(vk::CommandBuffer&) override {}
                void update(float fDeltaTime) override;

                void setControl(bool value) { bEnableControls = value; }
                void setActive(bool value) { bIsActive = value; }
                bool getIsActive() { return bIsActive; }

                void moveForward(bool bInv);
                void moveRight(bool bInv);
                void moveUp(bool bInv);

                void lookAt(float dX, float dY);

                glm::mat4 getProjection(bool flipY = false) const;
                glm::mat4 getView(bool flipY = false) const;

                glm::vec3 getForwardVector() const;
                glm::vec3 getUpVector() const;
                glm::vec3 getRightVector() const;

                const float getFieldOfView() const { return fieldOfView; }
                void setFieldOfView(float value) { fieldOfView = value; }

                const float getNearPlane() const { return nearPlane; }
                void setNearPlane(float value) { nearPlane = value; }

                const float getFarPlane() const { return farPlane; }
                void setFarPlane(float value) { farPlane = value; }

                const FFrustum& getFrustum() const { return frustum; }

                glm::vec4 viewPos{};

            private:
                FFrustum frustum;

                float dt{0.0}, fieldOfView{45.f}, nearPlane{0.5f}, farPlane{200.f}, sensitivity{15.f};
                float angleH{0.f}, angleV{0.f};
                bool bEnableControls{true}, bIsActive{false};
            };
        }
    }
}