#pragma once
#include "Resources/ResourceCunstruct.h"
#include "external/gltf/json.hpp"

namespace Engine
{
    void to_json(nlohmann::json &json, const FTextureCreateInfo &type);
    void from_json(const nlohmann::json &json, FTextureCreateInfo &type);

    void to_json(nlohmann::json &json, const FMaterialParams &type);
    void from_json(const nlohmann::json &json, FMaterialParams &type);

    void to_json(nlohmann::json &json, const FMaterialCreateInfo &type);
    void from_json(const nlohmann::json &json, FMaterialCreateInfo &type);

    void to_json(nlohmann::json &json, const FMeshCreateInfo &type);
    void from_json(const nlohmann::json &json, FMeshCreateInfo &type);
}