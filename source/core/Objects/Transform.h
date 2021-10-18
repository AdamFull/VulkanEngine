#pragma once

namespace Engine
{
    struct FTransform
    {
        glm::vec3 pos{};
        glm::vec3 rot{};
        glm::vec3 scale{1.f, 1.f, 1.f};

        FTransform& operator+=(const FTransform& rhs)
        {
            this->pos += rhs.pos;
            this->rot += rhs.rot;
            this->scale *= rhs.scale;
            return *this;
        }

        glm::mat4 GetModel();
        glm::mat3 GetNormal();
    };
}