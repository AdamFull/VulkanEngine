#pragma once
#include "Objects/RenderObject.h"

namespace Engine
{
    class CameraBase : public RenderObject
    {
    public:
        CameraBase() = default;
        explicit CameraBase(std::string srName) 
        {
            m_srName = srName;
        }
        
        void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void SetPerspectiveProjection(float fovy, float aspect, float near, float far);

        void Update(float fDeltaTime) override;

        const inline glm::mat4& GetProjection() const { return projectionMatrix; }
        const inline glm::mat4& GetView() const { return viewMatrix; }

    private:
        void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
        void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
        void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);

        glm::mat4 projectionMatrix{1.f};
        glm::mat4 viewMatrix{1.f};
    };
}