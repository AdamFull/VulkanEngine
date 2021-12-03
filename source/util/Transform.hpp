#pragma once

struct FTransform
{
    glm::vec3 pos{};
    glm::vec3 rot{};
    glm::vec3 scale{1.f, 1.f, 1.f};

    FTransform &operator+=(const FTransform &rhs)
    {
        this->pos += rhs.pos;
        this->rot += rhs.rot;
        this->scale *= rhs.scale;
        return *this;
    }

    const glm::vec3 GetForwardVector() const
    {
        float yaw = rot.y;
        float pitch = rot.x;
        return glm::vec3{sin(yaw), -pitch, cos(yaw)};
    }

    const glm::vec3 GetRightVector() const
    {
        return glm::vec3{0.f, -1.f, 0.f};
    }

    const glm::vec3 GetUpVector() const
    {
        auto forward = GetForwardVector();
        return glm::vec3{forward.z, 0.f, -forward.x};
    }

    inline glm::mat4 GetModel()
    {
        glm::mat4 translation{1.0};
        translation = glm::translate(translation, pos);
        // glm::radians(rot.x)
        if (rot.x != 0)
            translation = glm::rotate(translation, rot.x, glm::vec3(1.0, 0.0, 0.0));
        if (rot.y != 0)
            translation = glm::rotate(translation, rot.y, glm::vec3(0.0, 1.0, 0.0));
        if (rot.z != 0)
            translation = glm::rotate(translation, rot.z, glm::vec3(0.0, 0.0, 1.0));

        translation = glm::scale(translation, scale);

        return translation;
    }

    inline glm::mat4 GetNormal()
    {
        return glm::transpose(GetModel());
    }
};