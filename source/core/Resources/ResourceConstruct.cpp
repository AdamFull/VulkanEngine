#include "ResourceCunstruct.h"
#include "serializer/GLMSerialization.h"

namespace engine
{
    namespace resources
    {
        /*RESOURCES*/
        NLOHMANN_JSON_SERIALIZE_ENUM
        (
            ETextureType, 
            {
                {ETextureType::e2D, "2d"},
                {ETextureType::e3D, "3d"},
                {ETextureType::eCubemap, "cubemap"},
                {ETextureType::eArray, "array"}
            }
        )

        NLOHMANN_JSON_SERIALIZE_ENUM
        (
            ETextureAttachmentType, 
            {
                {ETextureAttachmentType::eDiffuseAlbedo, "color"},
                    {ETextureAttachmentType::eMetalicRoughness, "metalicRoughness"},
                    {ETextureAttachmentType::eSpecularGlossiness, "specularGloss"},
                    {ETextureAttachmentType::eEmissive, "emissive"},
                    {ETextureAttachmentType::eNormal, "normal"},
                    {ETextureAttachmentType::eHeight, "height"},
                    {ETextureAttachmentType::eOcclusion, "occlusion"},
                    {ETextureAttachmentType::eCubemap, "cubemap"},
                    {ETextureAttachmentType::eIrradiance, "irradiate"},
                    {ETextureAttachmentType::eBRDFLUT, "brdflut"},
                    {ETextureAttachmentType::ePrefiltred, "prefiltred"}
            }
        )

        NLOHMANN_JSON_SERIALIZE_ENUM
        (
            EMeshType, 
            {
                {EMeshType::eStatic, "static"},
                {EMeshType::eSkeletal, "skeletal"},
                {EMeshType::eGLTF, "gltf"}
            }
        )

        NLOHMANN_JSON_SERIALIZE_ENUM
        (
            ENoisePattern, 
            {
                {ENoisePattern::ePerlin, "perlin"},
                {ENoisePattern::eFractal, "fractal"}
            }
        )

        void to_json(nlohmann::json &json, const FNoiseParam &type)
        {
            json = nlohmann::json{
                {"pattern", type.pattern},
                {"width", type.width},
                {"height", type.height},
                {"depth", type.depth}};
        }

        void from_json(const nlohmann::json &json, FNoiseParam &type)
        {
            ParseArgument(json, type.pattern, "pattern");
            ParseArgument(json, type.width, "width");
            ParseArgument(json, type.height, "height");
            ParseArgument(json, type.depth, "depth");
        }

        void to_json(nlohmann::json &json, const FTextureCreateInfo &type)
        {
            json = nlohmann::json{
                {"name", type.srName},
                {"type", type.eType},
                {"attachment", type.attachment},
                {"src", type.srSrc},
                {"noise", type.noise}};
        }

        void from_json(const nlohmann::json &json, FTextureCreateInfo &type)
        {
            ParseArgument(json, type.srName, "name", true);
            ParseArgument(json, type.eType, "type", true);
            ParseArgument(json, type.attachment, "attachment", true);
            ParseArgument(json, type.srSrc, "src");
            ParseArgument(json, type.noise, "noise");
        }

        void to_json(nlohmann::json &json, const FMaterialParamsInfo &type)
        {
            json = nlohmann::json{
                {"cutoff", type.alpha_cutoff},
                {"metalic", type.metalic},
                {"roughness", type.specular}};
        }

        void from_json(const nlohmann::json &json, FMaterialParamsInfo &type)
        {
            ParseArgument(json, type.alpha_cutoff, "cutoff");
            ParseArgument(json, type.metalic, "metalic");
            ParseArgument(json, type.specular, "roughness");
        }

        void to_json(nlohmann::json &json, const FMaterialCreateInfo &type)
        {
            json = nlohmann::json{
                {"name", type.srName},
                {"textures", {type.vTextures}},
                {"params", type.fParams}};
        }

        void from_json(const nlohmann::json &json, FMaterialCreateInfo &type)
        {
            ParseArgument(json, type.srName, "name", true);
            ParseArgument(json, type.vTextures, "textures");
            ParseArgument(json, type.fParams, "params");
        }

        void to_json(nlohmann::json &json, const FMeshCreateInfo &type)
        {
            json = nlohmann::json{
                {"name", type.srName},
                {"type", type.eType},
                {"src", type.srSrc},
                {"use_included_material", type.bUseIncludedMaterial},
                {"materials", {type.vMaterials}}};
        }

        void from_json(const nlohmann::json &json, FMeshCreateInfo &type)
        {
            ParseArgument(json, type.srName, "name", true);
            ParseArgument(json, type.eType, "type", true);
            ParseArgument(json, type.srSrc, "src", true);
            ParseArgument(json, type.fRepeat, "repeat");
            ParseArgument(json, type.bUseIncludedMaterial, "use_included_material");
            if (!type.bUseIncludedMaterial)
                ParseArgument(json, type.vMaterials, "materials");
        }

        NLOHMANN_JSON_SERIALIZE_ENUM
        (
            ELightSourceType, 
            {
                {ELightSourceType::ePoint, "point"},
                {ELightSourceType::eSpot, "spot"},
                {ELightSourceType::eDirectional, "directional"}
            }
        )

        void to_json(nlohmann::json &json, const FLightCreateinfo &type)
        {
            json = nlohmann::json
            {
                {"name", type.srName},
                {"type", type.eType},
                {"color", type.vColor},
                {"intencity", type.fIntencity}
            };
        }

        void from_json(const nlohmann::json &json, FLightCreateinfo &type)
        {
            ParseArgument(json, type.srName, "name");
            ParseArgument(json, type.eType, "type", true);
            ParseArgument(json, type.vColor, "color", true);
            ParseArgument(json, type.fIntencity, "intencity");
        }
    }
}