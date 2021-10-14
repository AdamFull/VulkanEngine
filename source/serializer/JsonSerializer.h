#pragma once
#include "Resources/ResourceCunstruct.h"
#include "SceneConstruct.h"
#include "external/gltf/json.hpp"

namespace glm
{
    void to_json(nlohmann::json &json, const vec3 &type);
    void from_json(const nlohmann::json &json, vec3 &type);
}

namespace Engine
{
    /*RESOURCES*/
    void to_json(nlohmann::json &json, const FTextureCreateInfo &type);
    void from_json(const nlohmann::json &json, FTextureCreateInfo &type);

    void to_json(nlohmann::json &json, const FMaterialParams &type);
    void from_json(const nlohmann::json &json, FMaterialParams &type);

    void to_json(nlohmann::json &json, const FMaterialCreateInfo &type);
    void from_json(const nlohmann::json &json, FMaterialCreateInfo &type);

    void to_json(nlohmann::json &json, const FMeshCreateInfo &type);
    void from_json(const nlohmann::json &json, FMeshCreateInfo &type);

    /*SCENE*/
    void to_json(nlohmann::json &json, const FTransform &type);
    void from_json(const nlohmann::json &json, FTransform &type);

    void to_json(nlohmann::json &json, const FSceneObjectResourceBindings &type);
    void from_json(const nlohmann::json &json, FSceneObjectResourceBindings &type);

    void to_json(nlohmann::json &json, const FSceneObject &type);
    void from_json(const nlohmann::json &json, FSceneObject &type);

    void to_json(nlohmann::json &json, const FSceneCreateInfo &type);
    void from_json(const nlohmann::json &json, FSceneCreateInfo &type);
}