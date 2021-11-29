#pragma once
#include "Core/Scene/Objects/RenderObject.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            namespace Objects
            {
                namespace Components
                {
                    class CameraComponent : public RenderObject
                    {
                    public:
                        CameraComponent() = default;
                        explicit CameraComponent(std::string srName)
                        {
                            m_srName = srName;
                        }

                        void SetOrthographicProjection(float fLeft, float fRight, float fTop, float fBottom, float fNear, float fFar);
                        void SetPerspectiveProjection(float fovy, float aspect, float fNear, float fFar);

                        void Update(float fDeltaTime) override;

                        const inline glm::mat4 &GetProjection() const { return projectionMatrix; }
                        const inline glm::mat4 &GetView() const { return viewMatrix; }

                        glm::vec4 viewPos{};

                    private:
                        void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
                        void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
                        void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);

                        glm::mat4 projectionMatrix{1.f};
                        glm::mat4 viewMatrix{1.f};
                        bool bFlipY{true};
                    };
                }
            }
        }
    }
}