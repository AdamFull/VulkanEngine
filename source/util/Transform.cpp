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

glm::vec3 FTransform::getPosition() const
{
    return model[3];
}

void FTransform::setPosition(const glm::vec3& position)
{
    model = glm::translate(model, position);
}

glm::vec3 FTransform::getRotation() const
{
    return model[2];
}

void FTransform::setRotation(const glm::vec3& rotation)
{
    if (rotation.x != 0)
        model = glm::rotate(model, rotation.x, glm::vec3(1.0, 0.0, 0.0));
    if (rotation.y != 0)
        model = glm::rotate(model, rotation.y, glm::vec3(0.0, 1.0, 0.0));
    if (rotation.z != 0)
        model = glm::rotate(model, rotation.z, glm::vec3(0.0, 0.0, 1.0));
}

glm::vec3 FTransform::getScale() const
{
    return model[1];
}

void FTransform::setScale(const glm::vec3& _scale)
{
    model = glm::scale(model, _scale);
}

const glm::mat4& FTransform::getModel()
{
    setPosition(pos);
    setRotation(rot);
    setScale(scale);
    return model;
}

const glm::mat4& FTransform::getNormal()
{
    normal = glm::transpose(getModel());
    return normal;
}