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
                CCameraComponent();
                explicit CCameraComponent(std::string name)
                {
                    srName = name;
                }

                void update(float fDeltaTime) override;

                void moveForward(bool bInv);
                void moveRight(bool bInv);
                void moveUp(bool bInv);

                void lookAt(float dX, float dY);

                glm::mat4 getProjection() const;
                glm::mat4 getView() const;

                glm::vec3 getForwardVector() const;
                glm::vec3 getUpVector() const;
                glm::vec3 getRightVector() const;

                glm::vec4 viewPos{};

            private:
                float dt{0.0}, fieldOfView{45.f}, near{0.01f}, far{2048.f}, sensitivity{3.f};
                float angleH{0.f}, angleV{0.f};
                glm::vec3 direction{-1.0, 0.0, 0.0};
                //TODO: set projection type
            };
        }
    }
}