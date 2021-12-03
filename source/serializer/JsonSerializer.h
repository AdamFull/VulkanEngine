#pragma once
#include "Resources/ResourceCunstruct.h"
#include "SceneConstruct.h"
#include "GLMSerialization.h"

void to_json(nlohmann::json &json, const FTransform &type);
void from_json(const nlohmann::json &json, FTransform &type);

namespace Engine
{
    namespace Resources
    {
        /*RESOURCES*/
        void to_json(nlohmann::json &json, const FNoiseParam &type);
        void from_json(const nlohmann::json &json, FNoiseParam &type);

        void to_json(nlohmann::json &json, const FTextureCreateInfo &type);
        void from_json(const nlohmann::json &json, FTextureCreateInfo &type);

        void to_json(nlohmann::json &json, const FMaterialParamsInfo &type);
        void from_json(const nlohmann::json &json, FMaterialParamsInfo &type);

        void to_json(nlohmann::json &json, const FMaterialCreateInfo &type);
        void from_json(const nlohmann::json &json, FMaterialCreateInfo &type);

        void to_json(nlohmann::json &json, const FMeshCreateInfo &type);
        void from_json(const nlohmann::json &json, FMeshCreateInfo &type);
    }

    /*SCENE*/
    void to_json(nlohmann::json &json, const FSceneObject &type);
    void from_json(const nlohmann::json &json, FSceneObject &type);

    void to_json(nlohmann::json &json, const FSceneCreateInfo &type);
    void from_json(const nlohmann::json &json, FSceneCreateInfo &type);
}