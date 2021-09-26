#pragma once
#include "DataTypes/VulkanVertex.h"

namespace Engine
{
    class CameraBase // : public BaseObject
    {
    public:
        void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void SetPerspectiveProjection(float fovy, float aspect, float near, float far);

        void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
        void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
        void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);
        void SetViewYXZ(FTransform transformNew);

        //TODO: move this to base object in future
        const glm::vec3 GetForwardVector();
        const glm::vec3 GetRightVector();
        const glm::vec3 GetUpVector();
        inline const FTransform GetTransform() { return transform; }

        const inline glm::mat4& GetProjection() const { return projectionMatrix; }
        const inline glm::mat4& GetView() const { return viewMatrix; }

    private:
        glm::mat4 projectionMatrix{1.f};
        glm::mat4 viewMatrix{1.f};
        FTransform transform;
    };
}