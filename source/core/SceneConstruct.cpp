#include "SceneConstruct.h"
#include "serializer/GLMSerialization.h"

namespace Engine
{
    /*SCENE*/
    NLOHMANN_JSON_SERIALIZE_ENUM
    (
        ESceneObjectType,
        {
            {ESceneObjectType::eCamera, "camera"},
            {ESceneObjectType::eSkybox, "skybox"},
            {ESceneObjectType::eMeshComponent, "static_mesh"},
            {ESceneObjectType::eGltfMesh, "gltf_mesh"},
            {ESceneObjectType::eLightSource, "light_source"}
        }
    )


    void to_json(nlohmann::json &json, const FSceneObject &type)
    {
        json = nlohmann::json
        {
            {"name", type.srName},
            {"type", type.eObjectType},
            {"mesh", type.mesh},
            {"transform", type.fTransform},
            {"instances", type.vInstances},
            {"childs", {type.vSceneObjects}}
        };
    }

    void from_json(const nlohmann::json &json, FSceneObject &type)
    {
        // Required
        ParseArgument(json, type.srName, "name", true);
        ParseArgument(json, type.eObjectType, "type", true);
        ParseArgument(json, type.srUseVolume, "use_volume");

        // Optional
        ParseArgument(json, type.mesh, "mesh");
        ParseArgument(json, type.light, "light");
        ParseArgument(json, type.texture, "texture");
        ParseArgument(json, type.fTransform, "transform");
        ParseArgument(json, type.vInstances, "instances");
        ParseArgument(json, type.vSceneObjects, "childs");
    }

    void to_json(nlohmann::json &json, const FSceneCreateInfo &type)
    {
        json = nlohmann::json{
            {"skybox", type.skybox},
            {"objects", {type.vSceneObjects}}};
    }

    void from_json(const nlohmann::json &json, FSceneCreateInfo &type)
    {
        ParseArgument(json, type.skybox, "skybox");
        ParseArgument(json, type.vSceneObjects, "objects");
    }
}