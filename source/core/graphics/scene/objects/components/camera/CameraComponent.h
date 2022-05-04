#pragma once
#include "graphics/scene/objects/RenderObject.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            class CCameraComponent : public CRenderObject
            {
            public:
                CCameraComponent() = default;
                explicit CCameraComponent(std::string srName)
                {
                    m_srName = srName;
                }

                void setOrthographicProjection(float fLeft, float fRight, float fTop, float fBottom, float fNear, float fFar);
                void setPerspectiveProjection(float fovy, float aspect, float fNear, float fFar);

                void update(float fDeltaTime) override;

                const inline glm::mat4 &getProjection() const { return projectionMatrix; }
                const inline glm::mat4 &getView() const { return viewMatrix; }

                glm::vec4 viewPos{};

            private:
                void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
                void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
                void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

                glm::mat4 projectionMatrix{1.f};
                glm::mat4 viewMatrix{1.f};
                bool bFlipY{true};
            };
        }
    }
}