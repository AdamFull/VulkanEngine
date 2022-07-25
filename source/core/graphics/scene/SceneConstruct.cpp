#include "SceneConstruct.h"
#include "serializer/GLMSerialization.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            NLOHMANN_JSON_SERIALIZE_ENUM
            (
                ECullingType,
                {
                    {ECullingType::eByPoint, "point"},
                    {ECullingType::eBySphere, "sphere"},
                    {ECullingType::eByBox, "box"}
                }
            )

            void to_json(nlohmann::json &json, const FCullingInfo &type)
            {
                json = nlohmann::json
                {
                    {"type", type.eType},
                    {"enable", type.bEnableCulling},
                    {"radius", type.fSphereRadius}
                };
            }

            void from_json(const nlohmann::json &json, FCullingInfo &type)
            {
                // Optional
                ParseArgument(json, type.eType, "type");
                ParseArgument(json, type.bEnableCulling, "enable");
                ParseArgument(json, type.fSphereRadius, "radius");
            }


            void to_json(nlohmann::json &json, const FSceneObject &type)
            {
                json = nlohmann::json
                {
                    {"name", type.srName},
                    {"transform", type.fTransform},
                    {"culling", type.culling},
                    {"components", type.components},
                    {"childs", type.vSceneObjects}
                };
            }

            void from_json(const nlohmann::json &json, FSceneObject &type)
            {
                // Required
                ParseArgument(json, type.srName, "name", true);

                // Optional
                ParseArgument(json, type.fTransform, "transform");
                ParseArgument(json, type.culling, "culling");
                ParseArgument(json, type.components, "components");
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
    }
}