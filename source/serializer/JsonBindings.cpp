#include "JsonSerializer.h"

namespace Engine
{
    namespace Resources
    {
        /*RESOURCES*/
        NLOHMANN_JSON_SERIALIZE_ENUM(ETextureType, {{ETextureType::e2D, "2d"},
                                                    {ETextureType::e3D, "3d"},
                                                    {ETextureType::eCubemap, "cubemap"},
                                                    {ETextureType::eArray, "array"}})

        NLOHMANN_JSON_SERIALIZE_ENUM(ETextureAttachmentType, {{ETextureAttachmentType::eDiffuseAlbedo, "color"},
                                                              {ETextureAttachmentType::eMetalicRoughness, "metalicRoughness"},
                                                              {ETextureAttachmentType::eSpecularGlossiness, "specularGloss"},
                                                              {ETextureAttachmentType::eEmissive, "emissive"},
                                                              {ETextureAttachmentType::eNormal, "normal"},
                                                              {ETextureAttachmentType::eHeight, "height"},
                                                              {ETextureAttachmentType::eOcclusion, "occlusion"},
                                                              {ETextureAttachmentType::eCubemap, "cubemap"},
                                                              {ETextureAttachmentType::eIrradiance, "irradiate"},
                                                              {ETextureAttachmentType::eBRDFLUT, "brdflut"},
                                                              {ETextureAttachmentType::ePrefiltred, "prefiltred"}})

        NLOHMANN_JSON_SERIALIZE_ENUM(EMaterialType, {{EMaterialType::eUI, "ui"},
                                                     {EMaterialType::eDiffuse, "diffuse"},
                                                     {EMaterialType::eSkybox, "skybox"},
                                                     {EMaterialType::ePBR, "pbr"}})

        NLOHMANN_JSON_SERIALIZE_ENUM(EMeshType, {{EMeshType::eStatic, "static"},
                                                 {EMeshType::eSkeletal, "skeletal"},
                                                 {EMeshType::eGLTF, "gltf"}})

        NLOHMANN_JSON_SERIALIZE_ENUM(ENoisePattern, {{ENoisePattern::ePerlin, "perlin"},
                                                     {ENoisePattern::eFractal, "fractal"}})

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
            if (json.find("pattern") != json.end())
                json.at("pattern").get_to(type.pattern);

            if (json.find("width") != json.end())
                json.at("width").get_to(type.width);

            if (json.find("height") != json.end())
                json.at("height").get_to(type.height);

            if (json.find("depth") != json.end())
                json.at("depth").get_to(type.depth);
        }

        void to_json(nlohmann::json &json, const FTextureCreateInfo &type)
        {
            json = nlohmann::json{
                {"name", type.srName},
                {"type", type.eType},
                {"attachment", type.eAttachment},
                {"src", type.srSrc},
                {"noise", type.noise}};
        }

        void from_json(const nlohmann::json &json, FTextureCreateInfo &type)
        {
            // Required
            if (json.find("name") != json.end())
                json.at("name").get_to(type.srName);

            // Required
            if (json.find("type") != json.end())
                json.at("type").get_to(type.eType);

            // Required
            if (json.find("attachment") != json.end())
                json.at("attachment").get_to(type.eAttachment);

            // Optional
            if (json.find("src") != json.end())
                json.at("src").get_to(type.srSrc);

            // Optional
            if (json.find("noise") != json.end())
                json.at("noise").get_to(type.noise);
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
            // Optional
            if (json.find("cutoff") != json.end())
                json.at("cutoff").get_to(type.alpha_cutoff);

            // Optional
            if (json.find("metalic") != json.end())
                json.at("metalic").get_to(type.metalic);

            // Optional
            if (json.find("roughness") != json.end())
                json.at("roughness").get_to(type.specular);
        }

        void to_json(nlohmann::json &json, const FMaterialCreateInfo &type)
        {
            json = nlohmann::json{
                {"name", type.srName},
                {"primitive", type.srPrimitive},
                {"type", type.eType},
                {"attachments", type.srAttachments},
                {"textures", {type.vTextures}},
                {"params", type.fParams}};
        }

        void from_json(const nlohmann::json &json, FMaterialCreateInfo &type)
        {
            // Required
            if (json.find("name") != json.end())
                json.at("name").get_to(type.srName);

            // Required
            if (json.find("primitive") != json.end())
                json.at("primitive").get_to(type.srPrimitive);

            // Required
            if (json.find("type") != json.end())
                json.at("type").get_to(type.eType);

            // Optional
            if (json.find("attachments") != json.end())
                json.at("attachments").get_to(type.srAttachments);

            // Optional
            if (json.find("textures") != json.end())
                json.at("textures").get_to(type.vTextures);

            // Required
            if (json.find("params") != json.end())
                json.at("params").get_to(type.fParams);
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
            // Required
            if (json.find("name") != json.end())
                json.at("name").get_to(type.srName);

            // Required
            if (json.find("type") != json.end())
                json.at("type").get_to(type.eType);

            // Required
            if (json.find("src") != json.end())
                json.at("src").get_to(type.srSrc);

            if (json.find("repeat") != json.end())
                json.at("repeat").get_to(type.fRepeat);
            else
                type.fRepeat = 1.f;

            // Optional
            if (json.find("use_included_material") != json.end())
                json.at("use_included_material").get_to(type.bUseIncludedMaterial);

            if (!type.bUseIncludedMaterial)
            {
                // Optional
                if (json.find("materials") != json.end())
                    json.at("materials").get_to(type.vMaterials);
            }
        }
    }

    /*SCENE*/
    NLOHMANN_JSON_SERIALIZE_ENUM(ESceneObjectType,
                                 {{ESceneObjectType::eCamera, "camera"},
                                  {ESceneObjectType::eSkybox, "skybox"},
                                  {ESceneObjectType::eMeshComponent, "static_mesh"},
                                  {ESceneObjectType::eGltfMesh, "gltf_mesh"},
                                  {ESceneObjectType::eEnvironment, "environment"}})

    namespace Core
    {
        namespace Scene
        {
            namespace Objects
            {
                void to_json(nlohmann::json &json, const FTransform &type)
                {
                    json = nlohmann::json{
                        {"pos", type.pos},
                        {"rot", type.rot},
                        {"scale", type.scale}};
                }

                void from_json(const nlohmann::json &json, FTransform &type)
                {
                    // Optional
                    if (json.find("pos") != json.end())
                        json.at("pos").get_to(type.pos);

                    // Optional
                    if (json.find("rot") != json.end())
                        json.at("rot").get_to(type.rot);

                    // Optional
                    if (json.find("scale") != json.end())
                        json.at("scale").get_to(type.scale);
                }
            }
        }
    }

    void to_json(nlohmann::json &json, const FSceneObject &type)
    {
        json = nlohmann::json{
            {"name", type.srName},
            {"type", type.eObjectType},
            {"mesh", type.mesh},
            {"transform", type.fTransform},
            {"childs", {type.vSceneObjects}}};
    }

    void from_json(const nlohmann::json &json, FSceneObject &type)
    {
        // Required
        if (json.find("name") != json.end())
            json.at("name").get_to(type.srName);

        // Required
        if (json.find("type") != json.end())
            json.at("type").get_to(type.eObjectType);

        // Required
        if (json.find("use_volume") != json.end())
            json.at("use_volume").get_to(type.srUseVolume);

        // Optional
        if (json.find("mesh") != json.end())
            json.at("mesh").get_to(type.mesh);

        // Optional
        if (json.find("texture") != json.end())
            json.at("texture").get_to(type.texture);

        // Optional
        if (json.find("transform") != json.end())
            json.at("transform").get_to(type.fTransform);

        // Optional
        if (json.find("childs") != json.end())
            json.at("childs").get_to(type.vSceneObjects);
    }

    void to_json(nlohmann::json &json, const FSceneCreateInfo &type)
    {
        json = nlohmann::json{
            {"skybox", type.skybox},
            {"objects", {type.vSceneObjects}}};
    }

    void from_json(const nlohmann::json &json, FSceneCreateInfo &type)
    {
        // Optional
        if (json.find("skybox") != json.end())
            json.at("skybox").get_to(type.skybox);

        // Optional
        if (json.find("environment") != json.end())
            json.at("environment").get_to(type.environment);

        // Optional
        if (json.find("objects") != json.end())
            json.at("objects").get_to(type.vSceneObjects);

    }
}