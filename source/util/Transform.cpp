#include "transform.hpp"
#include "serializer/GLMSerialization.h"

void to_json(nlohmann::json &json, const FTransform &type)
{
    json = nlohmann::json{
        {"pos", type.pos},
        {"rot", type.rot},
        {"scale", type.scale}};
}

void from_json(const nlohmann::json &json, FTransform &type)
{
    ParseArgument(json, type.pos, "pos");
    ParseArgument(json, type.rot, "rot");
    ParseArgument(json, type.scale, "scale");
}

const glm::vec3& FTransform::getPosition() const
{
    return pos;
}

void FTransform::setPosition(const glm::vec3 &position)
{
    this->pos = position;
}

const glm::vec3& FTransform::getRotation() const
{
    return rot;
}

void FTransform::setRotation(const glm::vec3 &rotation)
{
    this->rot = rotation;
}

const glm::vec3& FTransform::getScale() const
{
    return scale;
}

void FTransform::setScale(const glm::vec3& scale)
{
    this->scale = scale;
}

const glm::mat4 FTransform::getModel()
{
    const float c3 = glm::cos(rot.z);
    const float s3 = glm::sin(rot.z);
    const float c2 = glm::cos(rot.x);
    const float s2 = glm::sin(rot.x);
    const float c1 = glm::cos(rot.y);
    const float s1 = glm::sin(rot.y);
    return glm::mat4{
        {
            scale.x * (c1 * c3 + s1 * s2 * s3),
            scale.x * (c2 * s3),
            scale.x * (c1 * s2 * s3 - c3 * s1),
            0.0f,
        },
        {
            scale.y * (c3 * s1 * s2 - c1 * s3),
            scale.y * (c2 * c3),
            scale.y * (c1 * c3 * s2 + s1 * s3),
            0.0f,
        },
        {
            scale.z * (c2 * s1),
            scale.z * (-s2),
            scale.z * (c1 * c2),
            0.0f,
        },
        {pos.x, pos.y, pos.z, 1.0f}};
}

const glm::mat4 FTransform::getNormal()
{
    const float c3 = glm::cos(rot.z);
    const float s3 = glm::sin(rot.z);
    const float c2 = glm::cos(rot.x);
    const float s2 = glm::sin(rot.x);
    const float c1 = glm::cos(rot.y);
    const float s1 = glm::sin(rot.y);
    const glm::vec3 invScale = 1.0f / scale;

    return glm::mat3{
        {
            invScale.x * (c1 * c3 + s1 * s2 * s3),
            invScale.x * (c2 * s3),
            invScale.x * (c1 * s2 * s3 - c3 * s1),
        },
        {
            invScale.y * (c3 * s1 * s2 - c1 * s3),
            invScale.y * (c2 * c3),
            invScale.y * (c1 * c3 * s2 + s1 * s3),
        },
        {
            invScale.z * (c2 * s1),
            invScale.z * (-s2),
            invScale.z * (c1 * c2),
        },
    };
}