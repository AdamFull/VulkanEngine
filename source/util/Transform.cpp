#include "Transform.hpp"
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