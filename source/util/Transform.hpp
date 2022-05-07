#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "serializer/serialization.hpp"

/**
 * @brief Structure that describes current transformations of render object
 * 
 */
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

    /**
     * @brief Get the Forward Vector of render object
     * 
     * @return const glm::vec3 Forward vector 
     */
    const glm::vec3 getForwardVector() const
    {
        glm::vec3 camFront;
		camFront.x = -cos(glm::radians(rot.x)) * sin(glm::radians(rot.y));
		camFront.y = sin(glm::radians(rot.x));
		camFront.z = cos(glm::radians(rot.x)) * cos(glm::radians(rot.y));
		camFront = glm::normalize(camFront);
        return camFront;
    }

    /**
     * @brief Get the Right Vector of render object
     * 
     * @return const glm::vec3 Right vector
     */
    const glm::vec3 getRightVector() const
    {
        auto camFront = getForwardVector();
        return glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    /**
     * @brief Get the Up Vector of render object
     * 
     * @return const glm::vec3 Up vector
     */
    const glm::vec3 getUpVector() const
    {
        return glm::vec3{0.f, 1.f, 0.f};
    }

    /**
     * @brief Get render object model matrix
     * 
     * @return glm::mat4 Model matrix
     */
    inline glm::mat4 getModel()
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

    /**
     * @brief Get render object normal matrix
     * 
     * @return glm::mat4 Normal matrix
     */
    inline glm::mat4 getNormal()
    {
        return glm::transpose(getModel());
    }
};

REGISTER_SERIALIZATION_BLOCK_H(FTransform);