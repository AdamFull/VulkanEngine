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
    glm::mat4 model{1.f}, normal{1.0};
    glm::vec3 direction{ 0.0f, 0.0f, 1.0f };

    FTransform &operator+=(const FTransform &rhs)
    {
        this->pos += rhs.pos;
        this->rot += rhs.rot;
        this->scale *= rhs.scale;
        return *this;
    }

    glm::vec3 getPosition() const;
    void setPosition(const glm::vec3& position);

    glm::vec3 getRotation() const;
    void setRotation(const glm::vec3& rotation);
    
    glm::vec3 getScale() const;
    void setScale(const glm::vec3& _scale);

    /**
     * @brief Get render object model matrix
     * 
     * @return glm::mat4 Model matrix
     */
    const glm::mat4& getModel();

    /**
     * @brief Get render object normal matrix
     * 
     * @return glm::mat4 Normal matrix
     */
    const glm::mat4& getNormal();
};

REGISTER_SERIALIZATION_BLOCK_H(FTransform);