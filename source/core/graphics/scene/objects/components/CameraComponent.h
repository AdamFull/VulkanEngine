#pragma once
#include "SceneComponent.h"

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
                void render(vk::CommandBuffer &commandBuffer) override {}
                void update(float fDeltaTime) override;
                void cleanup() override {}
                void destroy() override {}

                void setControl(bool bControl) { bEnableControls = bControl; }

                void moveForward(bool bInv);
                void moveRight(bool bInv);
                void moveUp(bool bInv);

                void lookAt(float dX, float dY);

                glm::mat4 getProjection() const;
                glm::mat4 getView() const;

                glm::vec3 getForwardVector() const;
                glm::vec3 getUpVector() const;
                glm::vec3 getRightVector() const;

                /**
                 * @brief Check is point in frustum
                 *
                 * @param pos Point position
                 * @return true when point is in frustum
                 */
                bool checkPoint(const glm::vec3 &pos);

                /**
                 * @brief Check is sphere intersects with frustum
                 *
                 * @param pos Sphere center position
                 * @param radius Sphere radius
                 * @return true when sphere intersects with camera frustum
                 */
                bool checkSphere(const glm::vec3 &pos, float radius);

                /**
                 * @brief Check is bounding box intersects with frustum
                 *
                 * @param start Bounding box start point
                 * @param end Bounding box end boint
                 * @return true when bounding box intersects with camera frustum
                 */
                bool checkBox(const glm::vec3 &start, const glm::vec3 &end);

                const float getFieldOfView() const { return fieldOfView; }
                const float getNearPlane() const { return nearPlane; }
                const float getFarPlane() const { return farPlane; }

                void setFieldOfView(float value) { fieldOfView = value; }
                void setNearPlane(float value) { nearPlane = value; }
                void setFarPlane(float value) { farPlane = value; }

                glm::vec4 viewPos{};

            private:
                void normalizeFrustumSide(size_t side);

                float dt{0.0}, fieldOfView{45.f}, nearPlane{0.1f}, farPlane{1000.f}, sensitivity{3.f};
                float angleH{0.f}, angleV{0.f};
                bool bEnableControls{true};

                // Frustum
                std::array<std::array<float, 4>, 6> frustumSides{};
                // TODO: set projection type
            };
        }
    }
}