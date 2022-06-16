#pragma once
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
        this->scale *= rhs.scale;
        this->pos = this->pos * rhs.scale + rhs.pos;
        this->rot += rhs.rot;
        return *this;
    }

    const glm::vec3& getPosition() const;
    void setPosition(const glm::vec3& position);

    const glm::vec3& getRotation() const;
    void setRotation(const glm::vec3& rotation);
    
    const glm::vec3& getScale() const;
    void setScale(const glm::vec3& _scale);

    /**
     * @brief Get render object model matrix
     * 
     * @return glm::mat4 Model matrix
     */
    const glm::mat4 getModel();

    /**
     * @brief Get render object normal matrix
     * 
     * @return glm::mat4 Normal matrix
     */
    const glm::mat4 getNormal();
};

REGISTER_SERIALIZATION_BLOCK_H(FTransform);